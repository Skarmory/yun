#include "input.h"

#include <stdbool.h>
#include <ncurses.h>

#include "player.h"
#include "map.h"
#include "util.h"

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
                went = move_mon(
                    you->mon,
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
