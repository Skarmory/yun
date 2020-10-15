#include "input.h"

#include "command.h"
#include "command_manager.h"
#include "input_keycodes.h"
#include "term.h"

#include <stdbool.h>
#include <stddef.h>

static enum CommandType _convert_command(struct Command* command)
{
    switch(command->cmd_char)
    {
        case KEYCODE_AT:
            return COMMAND_TYPE_DISPLAY_POSITION;
        case KEYCODE_h:
        case KEYCODE_j:
        case KEYCODE_k:
        case KEYCODE_l:
        case KEYCODE_y:
        case KEYCODE_u:
        case KEYCODE_b:
        case KEYCODE_n:
            return COMMAND_TYPE_MOVE;
        case KEYCODE_DOT:
        case KEYCODE_SPACE:
            return COMMAND_TYPE_PASS_TURN;
        case KEYCODE_q:
            return COMMAND_TYPE_SAVE_AND_QUIT;
        case KEYCODE_Q:
            return COMMAND_TYPE_NO_SAVE_AND_QUIT;
        case KEYCODE_c:
            return COMMAND_TYPE_DISPLAY_CHARACTER_SCREEN;
        case KEYCODE_COMMA:
            return COMMAND_TYPE_PICK_UP;
        case KEYCODE_i:
            return COMMAND_TYPE_DISPLAY_INVENTORY;
        case KEYCODE_SEMICOLON:
            return COMMAND_TYPE_LOOK_AT;
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
