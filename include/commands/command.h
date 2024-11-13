#ifndef YUN_COMMANDS_COMMAND_H
#define YUN_COMMANDS_COMMAND_H

typedef void(*command_execute_fn)(void* data);
typedef void(*command_data_free_fn)(void* cmd);

struct Command
{
    command_execute_fn   execute_func;
    command_data_free_fn free_func;
    void*                data;
};

void command_execute(const struct Command* cmd);

void commands_init(void);
void commands_uninit(void);

#endif
