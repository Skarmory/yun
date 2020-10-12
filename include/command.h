#ifndef NAXX_COMMAND_H
#define NAXX_COMMAND_H

#include "command_defs.h"

#include <stdbool.h>
#include <stddef.h>

struct Command
{
    char cmd_char;
    enum CommandType type;
};

struct CommandResult
{
    bool end_turn;
};

#endif
