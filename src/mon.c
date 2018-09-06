#include "mon.h"
#include <stdlib.h>
#include <stdio.h>
#include "util.h"
#include "map.h"
#include "message.h"
#include "log.h"
#include "mon_ai.h"

struct Mon* new_mon(int mtype, int x, int y)
{
    struct Mon* mon = (struct Mon*) malloc(sizeof(struct Mon));

    mon->next = NULL;
    mon->type = &mon_type[mtype];
    mon->x = x;
    mon->y = y;
    mon->pathing = mon->type->pathing;
    mon->weapon = NULL;

    mon->stats.primary = mon->type->primary;
    mon->stats.secondary = mon->type->secondary;
    mon->stats.tertiary = mon->type->tertiary;

    MSTAT(mon, stamina, max_health) = 1;
    MSTAT(mon, stamina, health) = 1;
    MSTAT(mon, intelligence, max_mana) = 1;
    MSTAT(mon, intelligence, mana) = 1;

    set_stat(mon, STRENGTH, mon->type->strength);
    set_stat(mon, AGILITY, mon->type->agility);
    set_stat(mon, INTELLIGENCE, mon->type->intelligence);
    set_stat(mon, SPIRIT, mon->type->spirit);
    set_stat(mon, STAMINA, mon->type->stamina);
    /*
    MSTAT(mon, strength, base_strength) = mon->type->strength;
    MSTAT(mon, agility, base_agility) = mon->type->agility;
    MSTAT(mon, intelligence, base_intelligence) = mon->type->intelligence;
    MSTAT(mon, spirit, base_spirit) = mon->type->spirit;
    MSTAT(mon, stamina, base_stamina) = mon->type->stamina;

    MSTAT(mon, strength, strength) = mon->type->strength;
    MSTAT(mon, agility, agility) = mon->type->agility;
    MSTAT(mon, intelligence, intelligence) = mon->type->intelligence;
    MSTAT(mon, spirit, spirit) = mon->type->spirit;
    MSTAT(mon, stamina, stamina) = mon->type->stamina;
    */

    return mon;
}

void destroy_mon(struct Mon* mon)
{
    rm_mon(mon);
    free(mon);
}

void update_mons(void)
{
    struct Mon* mon = cmap->monlist;

    while(mon)
    {
       update_mon_ai(mon);
       mon = mon->next;
    }
}

bool mon_has_pathing_attr(struct Mon* mon, int path_attr)
{
   return mon->pathing & path_attr;
}

struct Weapon* mon_get_weapon(struct Mon* mon)
{
    if(!mon->weapon)
        return mon->type->base_weapon;
    return mon->weapon;
}

bool mon_is_dead(struct Mon* mon)
{
    return HP(mon) <= 0;
}

void mon_chk_dead(struct Mon* mon)
{
    if(mon_is_dead(mon))
    {
        char buf[256];
        sprintf(buf, "The %s was slain.", mon->type->name);
        display_msg(buf);
        destroy_mon(mon);
    }
}
