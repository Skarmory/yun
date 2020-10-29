#include "console_commands_init.h"

#include "console_command.h"
#include "spawn.h"

const int c_console_commands_max = 32;

void init_console_commands(void)
{
    init_console_command_array(c_console_commands_max);

    init_spawn_console_command(g_console_commands);
}

void uninit_console_commands(void)
{
    uninit_console_command_array();
}

