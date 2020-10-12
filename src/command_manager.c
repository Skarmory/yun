#include "command_manager.h"

#include "command.h"
#include "command_defs.h"

#include <stdlib.h>

struct CommandManager* g_command_manager = NULL;

struct CommandManager* command_manager_new(void)
{
    struct CommandManager* cmd_command_manager = malloc(sizeof(struct CommandManager));
    cmd_command_manager->handler_count = 0;

    return cmd_command_manager;
}

void command_manager_free(struct CommandManager* manager)
{
    free(manager);
}

void command_manager_register_handler(struct CommandManager* command_manager, enum CommandType command_type, HandlerFunc handler_func)
{
    command_manager->handlers[command_manager->handler_count].cmd_type = command_type;
    command_manager->handlers[command_manager->handler_count].handler = handler_func;

    ++command_manager->handler_count;
}

struct CommandResult command_manager_handle_command(struct CommandManager* command_manager, struct Command* command)
{
    struct CommandResult result;
    result.end_turn = false;

    for(int i = 0; i < command_manager->handler_count; ++i)
    {
        if(command_manager->handlers[i].cmd_type == command->type)
        {
            command_manager->handlers[i].handler(command, &result);
        }
    }

    return result;
}
