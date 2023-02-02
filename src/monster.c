#include "monster.h"

#include "map.h"
#include "map_cell.h"
#include "message.h"
#include "mon_ai.h"
#include "mon_equip.h"
#include "mon_inventory.h"
#include "mon_type.h"
#include "object.h"
#include "obj_weapon.h"
#include "player.h"
#include "util.h"

#include <scieppend/core/geom.h>
#include <scieppend/core/log.h>

#include <stdio.h>
#include <stdlib.h>

/**
 * Create a monster of given type at given position and return it
 */
struct Mon* mon_new(struct MonType* type, int x, int y)
{
    struct Mon* mon = malloc(sizeof(struct Mon));

    mon->x = x;
    mon->y = y;
    mon->move_flags = type->move_flags;
    mon->type = type;
    mon->equipment = new_equipment();
    mon->inventory = new_inventory();
    mon->stats = type->base_stats;

    return mon;
}

/**
 * Destroy monster
 */
void mon_free(struct Mon* mon)
{
    free_equipment(mon->equipment);
    mon->equipment = NULL;
    free_inventory(mon->inventory);
    mon->inventory = NULL;
    free(mon);
}

/**
 * Return true if monster has ability to use a particular pathing type
 */
bool mon_has_move_attr(struct Mon* mon, MonAttrMoveFlags move_flags)
{
   return mon->move_flags & move_flags;
}

/**
 * Return true if this monster is dead
 */
bool mon_is_dead(struct Mon* mon)
{
    return mon->stats.hp <= 0;
}

/**
 * Return true if monster has two weapons equipped
 */
bool mon_dual_wielding(struct Mon* mon)
{
    return (mon->equipment->main_hand && mon->equipment->off_hand);
}

bool mon_shield_wielding(struct Mon* mon)
{
    return (mon->equipment->off_hand && mon->equipment->off_hand->objtype == OBJ_TYPE_ARMOUR);
}

/**
 * Check if monster is dead. If dead, print out death message and destroy it
 */
void mon_chk_dead(struct Mon* mon)
{
    if(mon_is_dead(mon))
    {
        display_fmsg_log("The %s was slain.", mon->type->name);

        map_cell_rm_mon(map_get_cell_by_world_coord(g_cmap, mon->x, mon->y), mon);
        mon_free(mon);
    }
}

/**
 * Get mon weapon if it has one, else return base montype weapon
 */
const struct Weapon* mon_get_weapon(struct Mon* mon)
{
    if(mon->equipment->main_hand)
    {
        return mon->equipment->main_hand;
    }

    return NULL;
}

bool mon_can_see(const struct Mon* mon, int x, int y)
{
    //struct MapCell* cell = map_get_cell_by_world_coord(g_cmap, mon->x, mon->y);

    if(geom_point_in_circle(x, y, mon->x, mon->y, mon->type->vision_radius))
    {
        int _x = mon->x;
        int _y = mon->y;
        float err = 0.0f;

        while(geom_gen_line_increment(mon->x, mon->y, x, y, &_x, &_y, &err))
        {
            //cell = map_get_cell_by_world_coord(g_cmap, _x, _y);
            //struct MapLocation* loc = map_cell_get_location(cell, _x, _y);
            struct MapLocation* loc = map_get_location(g_cmap, _x, _y);

            if(loc_blocks_sight(loc))
            {
                return (_x == x) && (_y == y);
            }
        }

        return true;
    }

    return false;
}

/**
 * Update all active mons
 */
void update_mons(void)
{
    struct ListNode* cell_node = NULL;
    list_for_each(&g_cmap->cell_list, cell_node)
    {
        struct MapCell* cell = cell_node->data;

        struct ListNode* node = NULL;
        list_for_each(&cell->mon_list, node)
        {
            update_mon_ai(node->data);
        }
    }
}
