#include "input.h"

#include "command.h"
#include "command_manager.h"
//#include "message.h"
//#include "mon_inventory.h"
#include "term.h"
//#include "ui.h"
//#include "ui_inventory.h"
//#include "ui_stats.h"
//#include "util.h"

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
            break;
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
    // loop until player has made some valid turn advancing input

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

        //char ch;
        //switch(ch = term_getch()) // get movement
        //{
        //    case '@':
        //        display_fmsg_nolog("Current position: %d, %d", you->mon->x, you->mon->y);
        //        break;
        //    case 'h':
        //    case 'j':
        //    case 'k':
        //    case 'l':
        //    case 'y':
        //    case 'u':
        //    case 'b':
        //    case 'n':
        //        // set went to true if move success
        //        went = _do_smart_action(
        //            you->mon->x + (ch == 'h' || ch == 'y' || ch == 'b' ? -1 : ch == 'l' || ch == 'u' || ch == 'n' ? 1 : 0),
        //            you->mon->y + (ch == 'k' || ch == 'y' || ch == 'u' ? -1 : ch == 'j' || ch == 'b' || ch == 'n' ? 1 : 0)
        //        );
        //        break;
        //    case '.':
        //        // Pass turn
        //        went = true;
        //        break;
        //    case 'q':
        //        if(prompt_yn("Really quit?"))
        //            do_quit();
        //        break;
        //    case 'm':
        //        display_msg_log("This is a message that is going to be larger than the display area of the message box so this ought be split up into multiple messages");
        //        break;
        //    case 'M':
        //        display_msg_log("This_is_a_message_that_is_going_to_be_larger_than_the_display_area_of_the_message_box_so_this_ought_be_split_up_into_multiple_messages");
        //        break;
        //    case 'c':
        //        went = character_screen_handler();
        //        break;
        //    case ',':
        //        went = _pick_up_object();
        //        break;
        //    case 'i':
        //        went = display_inventory_player();
        //        break;
        //    case ' ':
        //        went = true;
        //        break;
        //    default:
        //        continue;
        //}

        //if(!went)
        //{
        //    clear_msgs();
        //    flush_msg_buffer();
        //    display_main_screen();
        //}
    } while(true);
}
