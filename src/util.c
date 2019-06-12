#include "util.h"

#include "init.h"
#include "log.h"
#include "map.h"
#include "monster.h"
#include "mon_type.h"
#include "player.h"
#include "player_class.h"
#include "player_faction.h"
#include "player_race.h"
#include "term.h"
#include "ui.h"

#include <math.h>
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

int strrfindi(const char* haystack, const char needle, int start)
{
    int ret = -1;

    if(strlen(haystack) >= start)
    {
        const char* curr = haystack + start;

        do
        {
            if(*curr == needle)
                return (int)(curr - haystack) + 1;

            --curr;
        }
        while(curr >= haystack);
    }

    return ret;
}

/**
 * Handles game shut down. Destroys everything, ends ncurses, and prints a message out to stdout.
 */
void do_quit(void)
{
    char exit_msg[512];
    if(you && you->mon && mon_is_dead(you->mon))
        sprintf(exit_msg, "The dark void awaits you!");
    else
        sprintf(exit_msg, "Your curiosity will be the death of you.");

    free_player();

    if(cmap)
        free_map(cmap);

    uninit_naxx();
    puts(exit_msg);
    exit(0);
}

void sigint_handler(int _)
{
    if(prompt_yn("Really quit?"))
        do_quit();
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

/**
 * Compute log of x to a given base
 */
float log_base(int x, int base)
{
    return log(x) / log(base);
}
