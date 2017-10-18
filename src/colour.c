#include <ncurses.h>
#include "colour.h"

void init_colours(void)
{
    start_color();

    for(int i = 1; i < 256; ++i)
    {
        init_pair(i, i, COLOR_BLACK);
    }
}
