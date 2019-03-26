#include "player.h"

#include "map.h"
#include "message.h"
#include "monster.h"
#include "mon_type.h"
#include "player_class.h"
#include "player_race.h"
#include "ui.h"
#include "util.h"

#include <ncurses.h>
#include <stdlib.h>

struct Player* you = NULL;

/**
 * Create a new player.
 * There should only ever be one player (i.e. don't call this more than once)
 */
void new_player(void)
{
    you = (struct Player*)malloc(sizeof(struct Player));
    you->mon = new_mon(MT_HUMAN, 0, 0);
    you->name = NULL;
    you->faction = NULL;
    you->cls = NULL;
    you->race = NULL;
}

/**
 * Destroy the player
 */
void free_player(void)
{
    if(you != NULL)
    {
        you->faction = NULL;
        free(you->cls);
        free(you->race);
        list_rm(&cmap->mon_list, list_find(&cmap->mon_list, you->mon));
        free_mon(you->mon);
        free(you);
    }
}

/**
 * Check if the player is dead.
 * If so, print out death message and quit the game
 */
void player_chk_dead(void)
{
    if(mon_is_dead(you->mon))
    {
        clear_msgs();
        flush_msg_buffer();

        display_map();
        display_char_status();

        getch();
        display_msg_log("You died...");

        clear_msgs();
        flush_msg_buffer();

        getch();
        do_quit();
    }
}
