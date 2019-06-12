#include "monster.h"

#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "map.h"
#include "message.h"
#include "mon_ai.h"
#include "mon_equip.h"
#include "mon_inventory.h"
#include "mon_type.h"
#include "object.h"
#include "obj_weapon.h"
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
    mon->pathing = type->pathing;
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
 * Update all active mons
 */
void update_mons(void)
{
    ListNode* node;
    list_for_each(&cmap->mon_list, node)
    {
        update_mon_ai(node->data);
    }
}

/**
 * Return true if monster has ability to use a particular pathing type
 */
bool mon_has_pathing_attr(struct Mon* mon, int path_attr)
{
   return mon->pathing & path_attr;
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
        map_rm_mon(cmap, mon);
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
    return mon->type->base_weapon;
}
