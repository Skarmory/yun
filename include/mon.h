#ifndef MON_H
#define MON_H

#include "montype.h"
#include "attack_data.h"
#include "stats.h"

#define HP(mon) mon->stats.stamina.health
#define MAXHP(mon) mon->stats.stamina.max_health
#define MANA(mon) mon->stats.intelligence.mana
#define MAXMANA(mon) mon->stats.intelligence.max_mana

/* Struct that contains individual monster data */
struct Mon
{
    struct Mon* next; // linked list for mons on the level
    unsigned int x, y;
    int pathing;
    struct MonType* type;
    struct Weapon* weapon;
    struct Stats stats;
};

/* Create a monster of given type at given position and return it */
struct Mon* new_mon(int mtype, int x, int y);

/* Destroy monster */
void destroy_mon(struct Mon*);

/* Return true if monster has ability to use a particular pathing type */
bool mon_has_pathing_attr(struct Mon* mon, int path_attr);

/* Return monster's weapon */
struct Weapon* mon_get_weapon(struct Mon* mon);

/* Return true if this monster is dead */
bool mon_is_dead(struct Mon* mon);

/* Check if monster is dead. If dead, print out death message and destroy it */
void mon_chk_dead(struct Mon* mon);

#endif
