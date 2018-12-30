#include "mon.h"

#include <stdio.h>
#include <stdlib.h>

#include "equip.h"
#include "inventory.h"
#include "log.h"
#include "map.h"
#include "message.h"
#include "montype.h"
#include "mon_ai.h"
#include "util.h"
#include "weapon.h"

#define SET_MINION_STAT_SCALES(mon) \
    mon->stats.strength.scale = 0.5f; \
    mon->stats.agility.scale = 0.5f; \
    mon->stats.intelligence.scale = 0.5f; \
    mon->stats.spirit.scale = 0.5f; \
    mon->stats.stamina.scale = 0.5f;

/**
 * Create a monster of given type at given position and return it
 */
struct Mon* new_mon(int mtype, int x, int y)
{
    struct Mon* mon = (struct Mon*) malloc(sizeof(struct Mon));

    mon->next = NULL;
    mon->type = &mon_type[mtype];
    mon->x = x;
    mon->y = y;
    mon->pathing = mon->type->pathing;
    mon->equipment = new_equipment();
    mon->inventory = new_inventory();

    SET_MINION_STAT_SCALES(mon);

    MSTAT(mon, stamina, max_health) = 1;
    MSTAT(mon, stamina, health) = 1;
    MSTAT(mon, intelligence, max_mana) = 1;
    MSTAT(mon, intelligence, mana) = 1;

    set_stat(mon, STRENGTH, mon->type->strength);
    set_stat(mon, AGILITY, mon->type->agility);
    set_stat(mon, INTELLIGENCE, mon->type->intelligence);
    set_stat(mon, SPIRIT, mon->type->spirit);
    set_stat(mon, STAMINA, mon->type->stamina);

    return mon;
}

/**
 * Destroy monster
 */
void destroy_mon(struct Mon* mon)
{
    map_rm_mon(mon);
    free_equipment(mon->equipment);
    free(mon);
}

/**
 * Update all active mons
 */
void update_mons(void)
{
    struct Mon* mon = cmap->monlist;

    while(mon)
    {
        update_mon_ai(mon);
        mon = mon->next;
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

/**
 * Check if monster is dead. If dead, print out death message and destroy it
 */
void mon_chk_dead(struct Mon* mon)
{
    if(mon_is_dead(mon))
    {
        display_format_msg("The %s was slain.", mon->type->name);
        destroy_mon(mon);
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
