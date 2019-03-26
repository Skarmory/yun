#ifndef NAXX_MON_H
#define NAXX_MON_H

#include "defs.h"
#include "mon_stats.h"

struct MonType;
struct Equipment;
struct Inventory;
struct Weapon;

#define HP(mon) mon->stats.stamina.health
#define MAXHP(mon) mon->stats.stamina.max_health
#define MANA(mon) mon->stats.intelligence.mana
#define MAXMANA(mon) mon->stats.intelligence.max_mana

/**
 * Struct that contains individual monster data
 */
struct Mon
{
    unsigned int x, y;
    int pathing;
    const struct MonType* type;
    struct Equipment* equipment;
    struct Inventory* inventory;
    struct Stats stats;
};

struct Mon* new_mon(int mtype, int x, int y);
void free_mon(struct Mon*);
void update_mons(void);
bool mon_has_pathing_attr(struct Mon* mon, int path_attr);
bool mon_is_dead(struct Mon* mon);
bool mon_dual_wielding(struct Mon* mon);
bool mon_shield_wielding(struct Mon* mon);
void mon_chk_dead(struct Mon* mon);
const struct Weapon* mon_get_weapon(struct Mon* mon);

#endif
