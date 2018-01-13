#include "mon.h"
#include <stdlib.h>
#include <stdio.h>
#include "util.h"
#include "map.h"
#include "message.h"

struct Mon* new_mon(int mtype, int x, int y)
{
    struct Mon* mon = (struct Mon*) malloc(sizeof(struct Mon));

    mon->next = NULL;
    mon->type = &mon_type[mtype];
    mon->x = x;
    mon->y = y;
    mon->max_hp = 0;

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

    for(int hd = 0; hd < mon->type->num_hitdice; hd++)
    {
        mon->max_hp += random_int(1, mon->type->sides_per_hitdie);
    }

    mon->hp = mon->max_hp;

    return mon;
}

void destroy_mon(struct Mon* mon)
{
    rm_mon(mon);
    free(mon);

    char buf[256];
    sprintf(buf, "The %s was slain.", mon->type->name);
    display_msg(buf);
}

struct Weapon* get_weapon(struct Mon* mon)
{
    if(!mon->weapon)
        return mon->type->base_weapon;
    return mon->weapon;
}

void chk_dead(struct Mon* mon)
{
    if(mon->hp <= 0)
    {
        destroy_mon(mon);
    }
}
