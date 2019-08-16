#ifndef NAXX_TASKING_H
#define NAXX_TASKING_H

#include <stdbool.h>

typedef int(*task_func)(void*);
typedef int(*task_callback_func)(void*);

struct Tasker;
struct Task;

enum TaskStatus
{
    TASK_STATUS_NOT_STARTED,
    TASK_STATUS_EXECUTING,
    TASK_STATUS_SUCCESS,
    TASK_STATUS_FAILED
};

struct Tasker* tasker_new(void);
void tasker_free(struct Tasker* tasker);
void tasker_integrate(struct Tasker* tasker);
bool tasker_add_task(struct Tasker* tasker, struct Task* task);
bool tasker_has_pending_tasks(struct Tasker* tasker);
bool tasker_has_executing_tasks(struct Tasker* tasker);
bool tasker_has_completed_tasks(struct Tasker* tasker);

struct Task* task_new(task_func func, task_callback_func cb_func, void* args, int size_bytes);
bool task_free(struct Task* task);
task_func task_get_func(struct Task* task);
bool task_is_finished(struct Task* task);

#endif
