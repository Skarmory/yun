#include "util.h"

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "player.h"
#include "map.h"

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

void write_debug_msg(char* msg)
{
    FILE* debug_log = fopen("debug.log", "a");
    fprintf(debug_log, "%s\n", msg);
    fclose(debug_log);
}

void do_quit(void)
{
    destroy_player();
    destroy_map();
    endwin();
    use_default_colors();
    puts("Your curiosity will be the death of you.");
    exit(0);
}

int random_int(int lo, int hi)
{
    return lo + (rand() % (hi - lo + 1));
}
