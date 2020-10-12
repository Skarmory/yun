#include "tasking.h"

#include "list.h"
#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <string.h>
#include <threads.h>
#include <time.h>

struct Tasker* g_tasker = NULL;

enum TaskerState
{
    TASKER_STATE_EXECUTING,
    TASKER_STATE_STOPPING,
    TASKER_STATE_STOPPED
};

struct Tasker
{
    atomic_int  state;
    atomic_bool has_pending_tasks;
    atomic_bool has_executing_tasks;
    atomic_bool has_completed_tasks;

    thrd_t      thread;
    cnd_t       tasks_available_signal;
    mtx_t       pending_list_lock;
    mtx_t       complete_list_lock;

    List        task_list;
    List        pending_list;
    List        complete_list;
};

struct Task
{
    task_func          func;
    task_callback_func cb_func;
    void*              args;
    atomic_int         status;
};

/*
 * This is the "main loop" for the tasker thread.
 * It waits on a condition variable for tasks to be available.
 * Tasks are executed in a cooperative manner, so make sure your tasks are cooperative
 */
static int _tasker_update(struct Tasker* tasker)
{
    while(tasker->state == TASKER_STATE_EXECUTING)
    {
        // Add pending to executing
        if(tasker->has_pending_tasks)
        {
            mtx_lock(&tasker->pending_list_lock);

            ListNode *node, *next;
            list_for_each_safe(&tasker->pending_list, node, next)
            {
                list_splice_node(&tasker->pending_list, &tasker->task_list, node);
            }

            tasker->has_executing_tasks = true;
            tasker->has_pending_tasks = false;

            mtx_unlock(&tasker->pending_list_lock);
        }

        // Execute single pass of tasks
        if(tasker->has_executing_tasks)
        {
            ListNode *node, *next;
            list_for_each_safe(&tasker->task_list, node, next)
            {
                struct Task* task = node->data;
                task->status = task->func(task->args);

                // Task completed, switch it over to the complete list
                if(task->status == TASK_STATUS_SUCCESS || task->status == TASK_STATUS_FAILED)
                {
                    mtx_lock(&tasker->complete_list_lock);

                    list_splice_node(&tasker->task_list, &tasker->complete_list, node);
                    tasker->has_completed_tasks = true;

                    mtx_unlock(&tasker->complete_list_lock);

                    tasker->has_executing_tasks = (tasker->task_list.count > 0);
                }
            }
        }

        // Need to lock because we are checking for the absence of tasks.
        // A task could be added just after we check there are none
        // Therefore resulting in a deadlock
        mtx_lock(&tasker->pending_list_lock);
        if(!tasker->has_executing_tasks && !tasker->has_pending_tasks)
        {
#ifdef DEBUG
            log_msg(DEBUG, "No tasks available. Tasker thread going to sleep.");
#endif
            cnd_wait(&tasker->tasks_available_signal, &tasker->pending_list_lock);
        }
        mtx_unlock(&tasker->pending_list_lock);
    }

    tasker->state = TASKER_STATE_STOPPED;

    return 1;
}

struct Tasker* tasker_new(void)
{
    struct Tasker* tasker = malloc(sizeof(struct Tasker));
    tasker->state = TASKER_STATE_EXECUTING;
    tasker->has_pending_tasks = false;
    tasker->has_executing_tasks = false;
    tasker->has_completed_tasks = false;

    cnd_init(&tasker->tasks_available_signal);

    mtx_init(&tasker->pending_list_lock, mtx_plain);
    mtx_init(&tasker->complete_list_lock, mtx_plain);

    list_init(&tasker->pending_list);
    list_init(&tasker->task_list);
    list_init(&tasker->complete_list);

    thrd_create(&tasker->thread, (void*)_tasker_update, tasker);

    return tasker;
}

void tasker_free(struct Tasker* tasker)
{
    tasker->state = TASKER_STATE_STOPPING;
    while(tasker->state != TASKER_STATE_STOPPED)
    {
        // This is necessary to ensure the tasker thread does not deadlock by sleeping on the
        // condition variable whilst we block waiting for it to join.
        cnd_signal(&tasker->tasks_available_signal);
    }

    thrd_join(tasker->thread, NULL);
    mtx_destroy(&tasker->pending_list_lock);
    mtx_destroy(&tasker->complete_list_lock);
    cnd_destroy(&tasker->tasks_available_signal);

    ListNode *node, *next;
    list_for_each_safe(&tasker->pending_list, node, next)
    {
        task_free(node->data);
        free(node);
    }

    list_for_each_safe(&tasker->task_list, node, next)
    {
        task_free(node->data);
        free(node);
    }

    list_for_each_safe(&tasker->complete_list, node, next)
    {
        task_free(node->data);
        free(node);
    }

    free(tasker);
}


void tasker_integrate(struct Tasker* tasker)
{
    if(!tasker->has_completed_tasks)
        return;

    mtx_lock(&tasker->complete_list_lock);

    ListNode *node, *next;
    list_for_each_safe(&tasker->complete_list, node, next)
    {
        struct Task* task = node->data;
        if(task->cb_func)
            task->cb_func(task->args);

        list_rm(&tasker->complete_list, node);
        task_free(task);
    }

    mtx_unlock(&tasker->complete_list_lock);
}

bool tasker_add_task(struct Tasker* tasker, struct Task* task)
{
    if(task->status != TASK_STATUS_NOT_STARTED)
        return false;

    // Add task to pending list
    mtx_lock(&tasker->pending_list_lock);

    list_add(&tasker->pending_list, task);
    tasker->has_pending_tasks = true;

    mtx_unlock(&tasker->pending_list_lock);

    cnd_signal(&tasker->tasks_available_signal);

    return true;
}

void tasker_sync(struct Tasker* tasker)
{
    while(tasker->has_pending_tasks || tasker->has_executing_tasks)
    {
        // Wait for tasker idle
        // TODO: Wait on condition variable instead?
    }
}

bool tasker_has_pending_tasks(struct Tasker* tasker)
{
    return tasker->pending_task_count != 0;
}

bool tasker_has_executing_tasks(struct Tasker* tasker)
{
    return tasker->executing_task_count != 0;
}

bool tasker_has_completed_tasks(struct Tasker* tasker)
{
    return tasker->completed_task_count != 0;
}

struct Task* task_new(char* task_name, task_func func, task_func cb_func, void* args, int size_bytes)
{
    struct Task* task = malloc(sizeof(struct Task));
    task->status = TASK_STATUS_NOT_STARTED;
    task->func = func;
    task->cb_func = cb_func;
    task->args = malloc(size_bytes);
    memcpy(task->args, args, size_bytes);
    snprintf(task->name, sizeof(task->name), "%s", task_name);

    return task;
}

bool task_free(struct Task* task)
{
    if(task->status == TASK_STATUS_EXECUTING)
        return false;

    free(task->args);
    free(task);

    return true;
}

task_func task_get_func(struct Task* task)
{
    return task->func;
}

bool task_is_finished(struct Task* task)
{
    return task->status == TASK_STATUS_SUCCESS || task->status == TASK_STATUS_FAILED;
}
