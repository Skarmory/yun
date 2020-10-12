#include "tasking.h"

#include "list.h"
#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <string.h>
#include <threads.h>
#include <time.h>

#define MAX_THREADS 1

struct Tasker* g_tasker = NULL;

struct Thread;

static void _thread_init(struct Thread* thread, struct Tasker* tasker);
static void _thread_free(struct Thread* thread, struct Tasker* tasker);
static int  _thread_update(struct Thread* thread);

enum TaskerState
{
    TASKER_STATE_EXECUTING,
    TASKER_STATE_IDLE,
    TASKER_STATE_STOPPING,
    TASKER_STATE_STOPPED
};

enum ThreadState
{
    THREAD_STATE_EXECUTING,
    THREAD_STATE_IDLE,
    THREAD_STATE_STOPPING,
    THREAD_STATE_STOPPED
};

struct Thread
{
    struct Tasker* tasker;
    thrd_t         thread;
    cnd_t          signal;
    mtx_t          lock;
    atomic_int     state;
    struct Task*   task;
    int            id;
};

struct Tasker
{
    atomic_int    state;
    atomic_int    pending_task_count;
    atomic_int    executing_task_count;
    atomic_int    completed_task_count;

    struct Thread worker_threads[MAX_THREADS];
    thrd_t        thread;
    cnd_t         signal;
    mtx_t         lock;
    mtx_t         pending_list_lock;
    mtx_t         complete_list_lock;

    //List          task_list;
    List          pending_list;
    List          complete_list;
};

struct Task
{
    task_func          func;
    task_callback_func cb_func;
    void*              args;
    atomic_int         status;
    char               name[256];
};

static void _thread_init(struct Thread* thread, struct Tasker* tasker)
{
    thread->tasker = tasker;
    thread->state = THREAD_STATE_IDLE;
    mtx_init(&thread->lock, mtx_plain);
    cnd_init(&thread->signal);
    thrd_create(&thread->thread, (void*)_thread_update, thread);
}

static void _thread_free(struct Thread* thread, struct Tasker* tasker)
{
    while(thread->state != THREAD_STATE_STOPPED)
    {
        thread->state = THREAD_STATE_STOPPING;
        cnd_signal(&thread->signal);
    }

    thrd_join(thread->thread, NULL);
    cnd_destroy(&thread->signal);
    mtx_destroy(&thread->lock);
}

static int _thread_update(struct Thread* thread)
{
    while(thread->state != THREAD_STATE_STOPPING)
    {
        thread->state = THREAD_STATE_IDLE;
        log_format_msg(DEBUG, "Set thread %d to IDLE", thread->id);

        // Wait until signalled
        mtx_lock(&thread->lock);
        cnd_wait(&thread->signal, &thread->lock);

        // Signalled to do something now, so check for what's to be done

        if(thread->state == THREAD_STATE_STOPPING)
        {
            // Thread should stop, unlock and break
            mtx_unlock(&thread->lock);
            break;
        }

        thread->state = THREAD_STATE_EXECUTING;
        log_format_msg(DEBUG, "Set thread %d to EXECUTING", thread->id);

        // Lock the pending task list and check for tasks
        mtx_lock(&thread->tasker->pending_list_lock);
        {
            if(thread->tasker->pending_task_count == 0)
            {
                // No tasks, so just unlock and go back to wait state
                log_format_msg(DEBUG, "Thread %d found no tasks.", thread->id);
                mtx_unlock(&thread->tasker->pending_list_lock);
                continue;
            }

            // There is a task to do, grab it
            thread->task = list_pop_head(&thread->tasker->pending_list);
            --thread->tasker->pending_task_count;
            ++thread->tasker->executing_task_count;
        }
        mtx_unlock(&thread->tasker->pending_list_lock);

        log_format_msg(DEBUG, "Thread %d begin executing task: %s.", thread->id, thread->task->name);

        // Execute the task to completion
        thread->task->status = TASK_STATUS_EXECUTING;
        while(thread->task->status == TASK_STATUS_EXECUTING)
        {
            thread->task->status = thread->task->func(thread->task->args);
            log_format_msg(DEBUG, "Thread %d executing task: %s. Task status: %d", thread->id, thread->task->name, thread->task->status);
        }

        log_format_msg(DEBUG, "Thread %d finished executing task: %s.", thread->id, thread->task->name);

        mtx_lock(&thread->tasker->complete_list_lock);
        {
            // Task complete, lock complete list and add
            list_add(&thread->tasker->complete_list, thread->task);
            thread->task = NULL;
            --thread->tasker->executing_task_count;
            ++thread->tasker->completed_task_count;
        }
        mtx_unlock(&thread->tasker->complete_list_lock);

        mtx_unlock(&thread->lock);
    }

    thread->state = THREAD_STATE_STOPPED;
    return 1;
}

/*
 * This is the "main loop" for the tasker thread.
 * It waits on a condition variable for tasks to be available.
 * Tasks are executed in a cooperative manner, so make sure your tasks are cooperative
 */
static int _tasker_update(struct Tasker* tasker)
{
    while(tasker->state != TASKER_STATE_STOPPING)
    {
        tasker->state = TASKER_STATE_IDLE;

        mtx_lock(&tasker->lock);
        cnd_wait(&tasker->signal, &tasker->lock);

        if(tasker->state == TASKER_STATE_STOPPING)
        {
            mtx_unlock(&tasker->lock);
            break;
        }

        tasker->state = TASKER_STATE_EXECUTING;

        while(tasker->pending_task_count > 0)
        {
            for(int tidx = 0; tidx < MAX_THREADS; ++tidx)
            {
                if(tasker->worker_threads[tidx].state == THREAD_STATE_IDLE)
                {
                    // Found an idle thread, signal it

                    cnd_signal(&tasker->worker_threads[tidx].signal);
                    break;
                }
            }
        }

        mtx_unlock(&tasker->lock);
    }

    tasker->state = TASKER_STATE_STOPPED;

    return 1;
}

struct Tasker* tasker_new(void)
{
    struct Tasker* tasker = malloc(sizeof(struct Tasker));
    tasker->state = TASKER_STATE_IDLE;
    tasker->pending_task_count = 0;
    tasker->executing_task_count = 0;
    tasker->completed_task_count = 0;

    cnd_init(&tasker->signal);

    mtx_init(&tasker->lock, mtx_plain);
    mtx_init(&tasker->pending_list_lock, mtx_plain);
    mtx_init(&tasker->complete_list_lock, mtx_plain);

    thrd_create(&tasker->thread, (void*)_tasker_update, tasker);

    list_init(&tasker->pending_list);
    list_init(&tasker->complete_list);

    for(int i = 0; i < MAX_THREADS; ++i)
    {
        _thread_init(&tasker->worker_threads[i], tasker);
        tasker->worker_threads[i].id = i;
    }

    return tasker;
}

void tasker_free(struct Tasker* tasker)
{
    while(tasker->state != TASKER_STATE_STOPPED)
    {
        tasker->state = TASKER_STATE_STOPPING;
        cnd_signal(&tasker->signal);
    }

    for(int tidx = 0; tidx < MAX_THREADS; ++tidx)
    {
        _thread_free(&tasker->worker_threads[tidx], tasker);
    }

    mtx_destroy(&tasker->pending_list_lock);
    mtx_destroy(&tasker->complete_list_lock);
    mtx_destroy(&tasker->lock);
    cnd_destroy(&tasker->signal);

    ListNode *node, *next;
    list_for_each_safe(&tasker->pending_list, node, next)
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
    if(tasker->completed_task_count == 0)
    {
        return;
    }

    mtx_lock(&tasker->complete_list_lock);
    {
        ListNode *node, *next;
        list_for_each_safe(&tasker->complete_list, node, next)
        {
            struct Task* task = node->data;
            if(task->cb_func)
            {
                task->cb_func(task->args);
            }

            list_rm(&tasker->complete_list, node);
            task_free(task);
        }
    }
    mtx_unlock(&tasker->complete_list_lock);
}

bool tasker_add_task(struct Tasker* tasker, struct Task* task)
{
    if(task->status != TASK_STATUS_NOT_STARTED)
    {
        return false;
    }

    // Add task to pending list
    mtx_lock(&tasker->pending_list_lock);
    {
        list_add(&tasker->pending_list, task);
        ++tasker->pending_task_count;
    }
    mtx_unlock(&tasker->pending_list_lock);

    cnd_signal(&tasker->signal);

    return true;
}

void tasker_sync(struct Tasker* tasker)
{
    while(tasker->pending_task_count > 0 || tasker->executing_task_count > 0)
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
    {
        return false;
    }

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

