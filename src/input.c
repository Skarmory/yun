#include "input.h"

#include <stdbool.h>
#include <ncurses.h>

#include "player.h"
#include "map.h"
#include "util.h"
#include "attack.h"
#include "message.h"
#include "ui.h"

/**
 * Automatically determine the action of a player given a movement command
 *
 * Will attempt to attack a monster if possible, otherwise will attempt to move
 */
bool _do_smart_action(int x, int y)
{
    if(cmap->locs[x][y].mon == NULL)
        return move_mon(you->mon, x, y);
    else
        return do_player_attack_mon(cmap->locs[x][y].mon);

}

/**
 * Takes user input and calls the correct functionality
 *
 * Loops on a blocking input until user gives a command that causes a turn update
 */
void handle_input(void)
{
    // loop until player has made some valid turn advancing input
    bool went = false;
    do
    {
        char ch;
        switch(ch = getch()) // get movement
        {
            case 'h':
            case 'j':
            case 'k':
            case 'l':
            case 'y':
            case 'u':
            case 'b':
            case 'n':
                // set went to true if move success
                went = _do_smart_action(
                    you->mon->x + (ch == 'h' || ch == 'y' || ch == 'b' ? -1 : ch == 'l' || ch == 'u' || ch == 'n' ? 1 : 0),
                    you->mon->y + (ch == 'k' || ch == 'y' || ch == 'u' ? -1 : ch == 'j' || ch == 'b' || ch == 'n' ? 1 : 0)
                );
                break;
            case 'q':
                do_quit();
                break;
            case 'm':
                display_msg("This is a message that is going to be larger than the display area of the message box so this ought be split up into multiple messages");
                went=true;
                break;
            case 'M':
                display_msg("This_is_a_message_that_is_going_to_be_larger_than_the_display_area_of_the_message_box_so_this_ought_be_split_up_into_multiple_messages");
                went=true;
                break;
            case 'c':
                display_char_info_screen();
                break;
            default:
                break;

        }
    } while(!went);
}
