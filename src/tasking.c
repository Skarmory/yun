#include "tasking.h"

#include "list.h"
#include "log.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <string.h>
#include <threads.h>
#include <time.h>

#define MAX_THREADS 4

struct Tasker* g_tasker = NULL;

struct Thread;
static void _thread_init(struct Thread* thread, struct Tasker* tasker);
static void _thread_free(struct Thread* thread, struct Tasker* tasker);
static void _thread_start_task(struct Thread* thread, struct Task* task);
static void _thread_execute_task(struct Thread* thread);
static void _thread_end_task(struct Thread* thread);
static void _thread_stop(struct Thread* thread);
static int  _thread_update(struct Thread* thread);

static struct Thread* _tasker_get_idle_thread(struct Tasker* tasker);
static void           _tasker_execute_pending_tasks(struct Tasker* tasker);
static int            _tasker_update(struct Tasker* tasker);

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

/**
 * Set initial state for a worker thread.
 */
static void _thread_init(struct Thread* thread, struct Tasker* tasker)
{
    thread->tasker = tasker;
    thread->state = THREAD_STATE_IDLE;
    mtx_init(&thread->lock, mtx_plain);
    cnd_init(&thread->signal);
    thrd_create(&thread->thread, (void*)_thread_update, thread);
}

/**
 * Stop a worker thread, join, and destroy its members.
 */
static void _thread_free(struct Thread* thread, struct Tasker* tasker)
{
    _thread_stop(thread);

    while(thread->state != THREAD_STATE_STOPPED)
    {
        // Wait for thread to stop
    }

    thrd_join(thread->thread, NULL);
    cnd_destroy(&thread->signal);
    mtx_destroy(&thread->lock);
}

/**
 * SHOULD ONLY BE CALLED BY THE TASKER
 * Sets a thread's task and state and signals it to awaken.
 */
static void _thread_start_task(struct Thread* thread, struct Task* task)
{
    assert(thread->state == THREAD_STATE_IDLE);

    thread->task  = task;
    thread->state = THREAD_STATE_EXECUTING;
    ++thread->tasker->executing_task_count;

    cnd_signal(&thread->signal);
}

/**
 * SHOULD ONLY BE CALLED BY A WORKER THREAD
 * Loops executing the given task until task returns a non-executing state.
 */
static void _thread_execute_task(struct Thread* thread)
{
    log_format_msg(DEBUG, "Worker thread %d executing task: %s", thread->id, thread->task->name);
    thread->task->status = TASK_STATUS_EXECUTING;
    while(thread->task->status == TASK_STATUS_EXECUTING)
    {
        thread->task->status = thread->task->func(thread->task->args);
    }
}

/**
 * SHOULD ONLY BE CALLED BY A WORKER THREAD
 * Add a completed task to the tasker's completed task list.
 */
static void _thread_end_task(struct Thread* thread)
{
    log_format_msg(DEBUG, "Worker thread %d ending task: %s", thread->id, thread->task->name);

    mtx_lock(&thread->tasker->complete_list_lock);
    list_add(&thread->tasker->complete_list, thread->task);
    thread->task = NULL;
    --thread->tasker->executing_task_count;
    ++thread->tasker->completed_task_count;
    mtx_unlock(&thread->tasker->complete_list_lock);
    cnd_signal(&thread->tasker->signal);
}

/**
 * SHOULD ONLY BE CALLED BY THE TASKER
 * Acquire the thread's lock, set its state to stopping, and signal it to awaken.
 */
static void _thread_stop(struct Thread* thread)
{
    assert(thread->state != THREAD_STATE_STOPPED);

    mtx_lock(&thread->lock);
    assert(thread->state == THREAD_STATE_IDLE);
    thread->state = THREAD_STATE_STOPPING;
    mtx_unlock(&thread->lock);
    cnd_signal(&thread->signal);
}

/**
 * Main loop for a worker thread.
 * The thread will wait on a signal from the tasker to awaken.
 * If the thread has work it will remain locked until the work is complete and it returns to the idle state.
 */
static int _thread_update(struct Thread* thread)
{
    while(thread->state != THREAD_STATE_STOPPING)
    {
        mtx_lock(&thread->lock);
        thread->state = THREAD_STATE_IDLE;
        cnd_wait(&thread->signal, &thread->lock);

        switch(thread->state)
        {
            case THREAD_STATE_IDLE:
            case THREAD_STATE_STOPPING:
                break;
            case THREAD_STATE_EXECUTING:
                {
                    _thread_execute_task(thread);
                    _thread_end_task(thread);
                }
                break;
            case THREAD_STATE_STOPPED:
                assert(true); // Shouldn't hit this ever
                break;
        }

        mtx_unlock(&thread->lock);
    }

    thread->state = THREAD_STATE_STOPPED;
    return 1;
}

/**
 * Try to find an idle worker thread, if cannot find one then return NULL.
 */
static struct Thread* _tasker_get_idle_thread(struct Tasker* tasker)
{
    for(int tidx = 0; tidx < MAX_THREADS; ++tidx)
    {
        if(tasker->worker_threads[tidx].state == THREAD_STATE_IDLE)
        {
            return &tasker->worker_threads[tidx];
        }
    }

    return NULL;
}

/**
 * Trying to assign pending tasks to idle threads.
 * TODO: Make this more green?.. Can I be bothered to do that anyway?
 */
static void _tasker_execute_pending_tasks(struct Tasker* tasker)
{
        if(tasker->pending_task_count > 0)
        {
            log_format_msg(DEBUG, "Tasker found pending tasks: %d", tasker->pending_task_count);
        }

        while(tasker->pending_task_count > 0)
        {
            struct Thread* thread = _tasker_get_idle_thread(tasker);
            if(thread)
            {
                log_format_msg(DEBUG, "Tasker found idle worker thread: %d", thread->id);

                mtx_lock(&tasker->pending_list_lock);
                struct Task* task = list_pop_head(&tasker->pending_list);
                mtx_unlock(&tasker->pending_list_lock);

                _thread_start_task(thread, task);
                tasker->state = TASKER_STATE_EXECUTING;
                --tasker->pending_task_count;
            }
        }
}

/**
 * Main loop for the Tasker thread.
 * Wait on a signal from another thread adding a Task, or to stop.
 * Once a signal has been received, attempt to assign tasks to idle worker threads.
 */
static int _tasker_update(struct Tasker* tasker)
{
    while(tasker->state != TASKER_STATE_STOPPING)
    {
        mtx_lock(&tasker->lock);
        tasker->state = TASKER_STATE_IDLE;
        log_msg(DEBUG, "Tasker set idle");
        cnd_wait(&tasker->signal, &tasker->lock);

        log_msg(DEBUG, "Tasker received signal");

        if(tasker->state == TASKER_STATE_STOPPING)
        {
            log_msg(DEBUG, "Tasker thread breaking loop");
            mtx_unlock(&tasker->lock);
            break;
        }

        _tasker_execute_pending_tasks(tasker);

        mtx_unlock(&tasker->lock);
    }

    tasker->state = TASKER_STATE_STOPPED;

    return 1;
}

/**
 * Create a new tasker and set the initial state.
 * This creates the worker threads for the tasker.
 */
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

/**
 * Destroy the tasker and its internal state.
 * This also stops and joins the tasker's worker threads.
 */
void tasker_free(struct Tasker* tasker)
{
    tasker->state = TASKER_STATE_STOPPING;
    while(tasker->state != TASKER_STATE_STOPPED)
    {
        log_msg(DEBUG, "Trying to stop tasker");
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

/**
 * Execute the callback functions for completed tasks and clear them out.
 */
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

/**
 * Add a task to the tasker's list, and signal it to awaken.
 */
bool tasker_add_task(struct Tasker* tasker, struct Task* task)
{
    if(task->status != TASK_STATUS_NOT_STARTED)
    {
        return false;
    }

    // Add task to pending list
    mtx_lock(&tasker->lock);
    mtx_lock(&tasker->pending_list_lock);
    {
        list_add(&tasker->pending_list, task);
        ++tasker->pending_task_count;
    }
    mtx_unlock(&tasker->pending_list_lock);
    mtx_unlock(&tasker->lock);

    cnd_signal(&tasker->signal);

    return true;
}

/**
 * Block until tasker has finished executing all its pending tasks.
 */
void tasker_sync(struct Tasker* tasker)
{
    while(tasker->pending_task_count > 0 || tasker->executing_task_count > 0)
    {
        // Wait for tasker idle
        // TODO: Wait on condition variable instead?
    }
}

/**
 * Return true if tasker has any tasks that it has not started yet.
 */
bool tasker_has_pending_tasks(struct Tasker* tasker)
{
    return tasker->pending_task_count != 0;
}

/**
 * Return true if tasker has any tasks that it is currently executing.
 */
bool tasker_has_executing_tasks(struct Tasker* tasker)
{
    return tasker->executing_task_count != 0;
}

/**
 * Return true if tasker has any tasks that it has completed, awaiting integration.
 */
bool tasker_has_completed_tasks(struct Tasker* tasker)
{
    return tasker->completed_task_count != 0;
}

/**
 * Create a new task and set its initial state
 */
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

/**
 * Destroy a task.
 */
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

/**
 * Return the task's execution function.
 */
task_func task_get_func(struct Task* task)
{
    return task->func;
}

/**
 * Return true if the task has finished.
 */
bool task_is_finished(struct Task* task)
{
    return task->status == TASK_STATUS_SUCCESS || task->status == TASK_STATUS_FAILED;
}
