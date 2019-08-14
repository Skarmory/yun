#include "monster.h"

#include <stdio.h>
#include <stdlib.h>

#include "geom.h"
#include "log.h"
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

#define SET_MINION_STAT_SCALES(mon) \
    mon->stats.strength.scale = 0.5f; \
    mon->stats.agility.scale = 0.5f; \
    mon->stats.intelligence.scale = 0.5f; \
    mon->stats.spirit.scale = 0.5f; \
    mon->stats.stamina.scale = 0.5f;

/**
 * Create a monster of given type at given position and return it
 */
struct Mon* mon_new(struct MonType* type, int x, int y)
{
    struct Mon* mon = malloc(sizeof(struct Mon));

    mon->type = type;
    mon->x = x;
    mon->y = y;
    mon->move_flags = type->move_flags;
    mon->equipment = new_equipment();
    mon->inventory = new_inventory();

    SET_MINION_STAT_SCALES(mon);

    MSTAT(mon, stamina, max_health) = 1;
    MSTAT(mon, stamina, health) = 1;
    MSTAT(mon, intelligence, max_mana) = 1;
    MSTAT(mon, intelligence, mana) = 1;

    set_stat(mon, STAT_STRENGTH, type->strength);
    set_stat(mon, STAT_AGILITY, type->agility);
    set_stat(mon, STAT_INTELLIGENCE, type->intelligence);
    set_stat(mon, STAT_SPIRIT, type->spirit);
    set_stat(mon, STAT_STAMINA, type->stamina);

    return mon;
}

/**
 * Destroy monster
 */
void mon_free(struct Mon* mon)
{
    free_equipment(mon->equipment);
    free_inventory(mon->inventory);
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
    return HP(mon) <= 0;
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

        map_cell_rm_mon(map_get_cell_by_world_coord(cmap, mon->x, mon->y), mon);
        mon_free(mon);
    }
}

/**
 * Get mon weapon if it has one, else return base montype weapon
 */
const struct Weapon* mon_get_weapon(struct Mon* mon)
{
    if(mon->equipment->main_hand)
        return mon->equipment->main_hand;
    return NULL;
}

bool mon_move(struct Mon* mon, int newx, int newy)
{
    struct MapCell* old_cell = map_get_cell_by_world_coord(cmap, mon->x, mon->y);
    struct MapCell* new_cell = map_get_cell_by_world_coord(cmap, newx, newy);

    if(!map_cell_is_valid_move(new_cell, newx, newy, mon->move_flags))
        return false;

    struct MapLocation* old_loc = map_cell_get_location(old_cell, mon->x, mon->y);
    struct MapLocation* new_loc = map_cell_get_location(new_cell, newx, newy);

    old_loc->mon = NULL;
    new_loc->mon = mon;
    mon->x = newx;
    mon->y = newy;

    if(old_cell != new_cell)
    {
        list_rm(&old_cell->mon_list, list_find(&old_cell->mon_list, mon));
        list_add(&new_cell->mon_list, mon);
    }

    return true;
}

bool mon_can_see(struct Mon* mon, int x, int y)
{
    struct MapCell* cell = map_get_cell_by_world_coord(cmap, mon->x, mon->y);
    struct MapLocation* loc = map_cell_get_location(cell, mon->x, mon->y);

    if(x >= mon->x - mon->type->vision_radius && x <= mon->x + mon->type->vision_radius &&
       y >= mon->y - mon->type->vision_radius && y <= mon->y + mon->type->vision_radius)
    {
        int _x = mon->x;
        int _y = mon->y;
        float err = 0.0f;

        while(gen_line_increment(mon->x, mon->y, x, y, &_x, &_y, &err))
        {
            cell = map_get_cell_by_world_coord(cmap, _x, _y);
            loc = map_cell_get_location(cell, _x, _y);

            if(loc_blocks_sight(loc))
            {
                if(_x == x && _y == y)
                    return true;
                return false;
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
    ListNode* cell_node;
    list_for_each(&cmap->cell_list, cell_node)
    {
        struct MapCell* cell = cell_node->data;

        ListNode* node;
        list_for_each(&cell->mon_list, node)
        {
            update_mon_ai(node->data);
        }
    }
}
