#ifndef NAXX_GAMEPLAY_H
#define NAXX_GAMEPLAY_H

#include "command_defs.h"

struct Command;
struct CommandResult;

void gameplay_command_handler_func(struct Command* cmd, struct CommandResult* cmd_res);

#endif
