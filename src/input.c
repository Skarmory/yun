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
                // set went to true if move success
                went = move_mon(
                    you->mon,
                    you->mon->x + (ch == 'h' ? -1 : ch == 'l' ? 1 : 0),
                    you->mon->y + (ch == 'k' ? -1 : ch == 'j' ? 1 : 0)
                );
                break;
            case 'q':
                do_quit();
                break;
        }
    } while(!went);
}
