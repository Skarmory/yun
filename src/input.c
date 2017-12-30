#include "input.h"

#include <stdbool.h>
#include <ncurses.h>

#include "player.h"
#include "map.h"
#include "util.h"
#include "attack.h"
#include "message.h"

bool _do_smart_action(int x, int y)
{
    if(cmap->locs[x][y].mon == NULL)
        return move_mon(you->mon, x, y);
    else
        return do_attack(you->mon, cmap->locs[x][y].mon);

}

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
                clear_msgs();
                went = _do_smart_action(
                    you->mon->x + (ch == 'h' || ch == 'y' || ch == 'b' ? -1 : ch == 'l' || ch == 'u' || ch == 'n' ? 1 : 0),
                    you->mon->y + (ch == 'k' || ch == 'y' || ch == 'u' ? -1 : ch == 'j' || ch == 'b' || ch == 'n' ? 1 : 0)
                );
                break;
            case 'q':
                do_quit();
                break;
        }
    } while(!went);
}
