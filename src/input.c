#include "input.h"

#include "command.h"
#include "command_manager.h"
#include "term.h"

#include <stdbool.h>
#include <stddef.h>

static enum CommandType _convert_command(struct Command* command)
{
    switch(command->cmd_char)
    {
        case '@':
            return COMMAND_TYPE_DISPLAY_POSITION;
        case 'h':
        case 'j':
        case 'k':
        case 'l':
        case 'y':
        case 'u':
        case 'b':
        case 'n':
            return COMMAND_TYPE_MOVE;
        case '.':
        case ' ':
            return COMMAND_TYPE_PASS_TURN;
        case 'q':
            return COMMAND_TYPE_SAVE_AND_QUIT;
        case 'Q':
            return COMMAND_TYPE_NO_SAVE_AND_QUIT;
        case 'c':
            return COMMAND_TYPE_DISPLAY_CHARACTER_SCREEN;
        case ',':
            return COMMAND_TYPE_PICK_UP;
        case 'i':
            return COMMAND_TYPE_DISPLAY_INVENTORY;
        default:
            return COMMAND_TYPE_UNKNOWN;
    }
}

/**
 * Takes user input and calls the correct functionality
 *
 * Loops on a blocking input until user gives a command that causes a turn update
 */
void handle_input(void)
{
    // Loop until player has made some valid turn advancing input
    do
    {
        struct Command command;
        command.cmd_char = term_getch();
        command.type = _convert_command(&command);

        struct CommandResult result = command_manager_handle_command(g_command_manager, &command);

        if(result.end_turn)
        {
            break;
        }
    }
    while(true);
}
