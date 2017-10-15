#include "util.h"

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

void do_quit(void)
{
    endwin();
    use_default_colors();
    puts("Your curiosity will be the death of you.");
    exit(0);
}
