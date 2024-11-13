#include "commands/command.h"

#include <scieppend/core/array.h>

static struct _CommandExecutor
{
    struct Array commands_array;
} _cmd_exe;

void command_execute(const struct Command* cmd)
{
    cmd->execute_func(cmd->data);
    array_add(&_cmd_exe.commands_array, &cmd);
}

void commands_init(void)
{
    array_init(&_cmd_exe.commands_array, sizeof(const struct Command*), 1024, NULL, NULL);
}

void commands_uninit(void)
{
    array_uninit(&_cmd_exe.commands_array);
}
