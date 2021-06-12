#ifndef YUN_MON_H
#define YUN_MON_H

#include "mon_attr.h"
#include "mon_stats.h"

struct MonType;
struct Equipment;
struct Inventory;
struct Weapon;

/**
 * Struct that contains individual monster data
 */
struct Mon
{
    int x;
    int y;
    MonAttrMoveFlags move_flags;
    const struct MonType* type;
    struct Equipment* equipment;
    struct Inventory* inventory;
    struct Stats stats;
};

struct Mon* mon_new(struct MonType* type, int x, int y);
void mon_free(struct Mon*);
bool mon_has_move_attr(struct Mon* mon, MonAttrMoveFlags move_flags);
bool mon_is_dead(struct Mon* mon);
bool mon_dual_wielding(struct Mon* mon);
bool mon_shield_wielding(struct Mon* mon);
void mon_chk_dead(struct Mon* mon);
const struct Weapon* mon_get_weapon(struct Mon* mon);
bool mon_can_see(const struct Mon* mon, int x, int y);

void update_mons(void);

#endif
