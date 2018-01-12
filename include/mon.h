#ifndef MON_H
#define MON_H

#include "montype.h"
#include "attack_data.h"
#include "stats.h"

struct Mon
{
    struct Mon* next; // linked list for mons on the level
    unsigned int x, y;
    struct MonType* type;
    struct Weapon* weapon;
    int hp;
    int max_hp;
    struct Stats stats;
};

struct Mon* new_mon(int mtype, int x, int y);
void destroy_mon(struct Mon*);
struct Weapon* get_weapon(struct Mon* mon);
void chk_dead(struct Mon* mon);

#endif
