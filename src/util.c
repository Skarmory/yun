#include "util.h"

#include "class.h"
#include "faction.h"
#include "log.h"
#include "map.h"
#include "mon.h"
#include "player.h"
#include "race.h"

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Fills in the special format character with the correct game specific string
 */
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

/**
 * Handles game shut down. Destroys everything, ends ncurses, and prints a message out to stdout.
 */
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

/**
 * Get a random int between given range inclusive
 */
int random_int(int lo, int hi)
{
    return lo + (rand() % (hi - lo + 1));
}

/**
 * Get a number between 1 and 100
 */
int roll_d100(void)
{
    return random_int(1, 100);
}

/**
 * Do percentile roll and return value between 0.01 and 1.0
 */
float roll_d100f(void)
{
    return (float)rand() / (float)RAND_MAX;
}
