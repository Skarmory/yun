#include "util.h"

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "player.h"
#include "map.h"
#include "log.h"

void convert_arg(char c, char* buf)
{
    switch(c)
    {
        case 'C':
            strcpy(buf, you->cls->name_plural);
            return;
        case 'c':
            strcpy(buf, you->cls->name);
            return;
        case 'F':
            strcpy(buf, you->race->faction == FA_ALLIANCE ? "The Alliance" : "The Horde");
            return;
        case 'H':
            strcpy(buf, you->race->home);
            return;
        case 'L':
            strcpy(buf, you->race->leader);
            return;
    }
}

void do_quit(void)
{
    char exit_msg[512];
    if(mon_is_dead(you->mon))
        sprintf(exit_msg, "The dark void awaits you!");
    else
        sprintf(exit_msg, "Your curiosity will be the death of you.");

    destroy_player();
    destroy_map();
    destroy_logs();
    endwin();
    use_default_colors();
    puts(exit_msg);
    exit(0);
}

int random_int(int lo, int hi)
{
    return lo + (rand() % (hi - lo + 1));
}
