#include "colour.h"

#include <ncurses.h>

/**
 * Enable ncurses colours and create 256 colour pairs where background is black
 */
void init_colours(void)
{
    start_color();

    for(int i = 1; i < 256; ++i)
    {
        init_pair(i, i, COLOR_BLACK);
    }
}
