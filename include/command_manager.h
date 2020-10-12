#ifndef NAXX_COMMAND_MANAGER_H
#define NAXX_COMMAND_MANAGER_H

#include "command_defs.h"

#define COMMAND_HANDLERS_MAX 32

struct Command;

struct CommandHandler
{
    enum CommandType cmd_type;
    HandlerFunc handler;
};

struct CommandManager
{
    struct CommandHandler handlers[COMMAND_HANDLERS_MAX];
    int handler_count;
};

struct CommandManager* command_manager_new(void);
void command_manager_free(struct CommandManager* manager);
void command_manager_register_handler(struct CommandManager* command_manager, enum CommandType command_type, HandlerFunc handler_func);
struct CommandResult command_manager_handle_command(struct CommandManager* command_manager, struct Command* command);

extern struct CommandManager* g_command_manager;

#endif
