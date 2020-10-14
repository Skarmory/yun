#include "player.h"

#include "map.h"
#include "map_cell.h"
#include "message.h"
#include "monster.h"
#include "mon_type.h"
#include "player_class.h"
#include "player_race.h"
#include "term.h"
#include "ui.h"
#include "util.h"

#include <stdlib.h>

struct Player* you = NULL;

/**
 * Create a new player.
 * There should only ever be one player (i.e. don't call this more than once)
 */
void new_player(void)
{
    you = (struct Player*)malloc(sizeof(struct Player));
    you->mon = mon_new(mon_type_lookup_by_name("human"), 0, 0);
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
        if(cmap)
        {
            struct MapCell* player_cell = map_get_cell_by_world_coord(cmap, you->mon->x, you->mon->y);
            list_rm(&player_cell->mon_list, list_find(&player_cell->mon_list, you->mon));
        }
        mon_free(you->mon);
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

        term_getch();
        display_msg_log("You died...");

        clear_msgs();
        flush_msg_buffer();

        term_getch();
        do_quit();
    }
}
