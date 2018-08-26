#ifndef MON_H
#define MON_H

#include "montype.h"
#include "attack_data.h"
#include "stats.h"

#define HP(mon) mon->stats.stamina.health
#define MAXHP(mon) mon->stats.stamina.max_health
#define MANA(mon) mon->stats.intelligence.mana
#define MAXMANA(mon) mon->stats.intelligence.max_mana

struct Mon
{
    struct Mon* next; // linked list for mons on the level
    unsigned int x, y;
    int pathing;
    struct MonType* type;
    struct Weapon* weapon;
    struct Stats stats;
};

struct Mon* new_mon(int mtype, int x, int y);
void destroy_mon(struct Mon*);

bool mon_has_pathing_attr(struct Mon* mon, int path_attr);

struct Weapon* get_weapon(struct Mon* mon);
void chk_dead(struct Mon* mon);

#endif
