#include "player.h"

#include "map.h"
#include "map_cell.h"
#include "message.h"
#include "monster.h"
#include "mon_type.h"
#include "player_class.h"
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
    you->name = NULL;
    you->cls  = NULL;
    you->mon = mon_new(mon_type_look_up_by_id("humn"), 0, 0);
}

/**
 * Destroy the player
 */
void free_player(void)
{
    if(you != NULL)
    {
        free(you->cls);
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

        term_wait_on_input();
        display_msg_log("You died...");

        clear_msgs();
        flush_msg_buffer();

        term_wait_on_input();
        do_quit();
    }
}
