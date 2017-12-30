#ifndef MON_H
#define MON_H

#include "montype.h"
#include "attack_data.h"

struct Mon
{
    struct Mon* next; // linked list for mons on the level
    unsigned int x, y;
    struct MonType* type;
    struct Weapon* weapon;
    int hp;
    int max_hp;
};

struct Mon* gen_mon(int mtype, int x, int y);
void kill_mon(struct Mon*);
struct Weapon* get_weapon(struct Mon* mon);
void chk_dead(struct Mon* mon);

#endif
