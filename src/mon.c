#include "mon.h"
#include <stdlib.h>
#include "util.h"
#include "map.h"

struct Mon* gen_mon(int mtype, int x, int y)
{
    struct Mon* mon = (struct Mon*) malloc(sizeof(struct Mon));

    mon->next = NULL;
    mon->type = &mon_type[mtype];
    mon->x = x;
    mon->y = y;
    mon->max_hp = 0;

    for(int hd = 0; hd < mon->type->num_hitdice; hd++)
    {
        mon->max_hp += random_int(1, mon->type->sides_per_hitdie);
    }

    mon->hp = mon->max_hp;

    return mon;
}

void kill_mon(struct Mon* mon)
{
    rm_mon(mon);
    free(mon);
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
        kill_mon(mon);
    }
}
