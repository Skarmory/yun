#include "player.h"

#include "map.h"
#include "map_cell.h"
#include "message.h"
#include "monster.h"
#include "mon_type.h"
#include "player_class.h"
#include "ui.h"
#include "util.h"

#include <scieppend/core/term.h>

#include <stdlib.h>

struct Player* g_you = NULL;

/**
 * Create a new player.
 * There should only ever be one player (i.e. don't call this more than once)
 */
void new_player(void)
{
    g_you = malloc(sizeof(struct Player));
    g_you->name = NULL;
    g_you->cls  = NULL;
    g_you->mon = mon_new(mon_type_look_up_by_id("humn"), 0, 0);
}

/**
 * Destroy the player
 */
void free_player(void)
{
    if(g_you != NULL)
    {
        free(g_you->cls);
        if(g_cmap)
        {
            struct MapCell* player_cell = map_get_cell_by_world_coord(g_cmap, g_you->mon->x, g_you->mon->y);
            list_rm(&player_cell->mon_list, list_find(&player_cell->mon_list, g_you->mon));
        }
        mon_free(g_you->mon);
        free(g_you);
    }
}

/**
 * Check if the player is dead.
 * If so, print out death message and quit the game
 */
void player_chk_dead(void)
{
    if(mon_is_dead(g_you->mon))
    {
        display_main_screen();

        clear_msgs();
        flush_msg_buffer();
        term_refresh();

        term_wait_on_input();
        display_msg_log("You died...");

        clear_msgs();
        flush_msg_buffer();
        term_refresh();

        term_wait_on_input();
        do_quit();
    }
}
