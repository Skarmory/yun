#ifndef YUN_MONTYPE_H
#define YUN_MONTYPE_H

#include "mon_attr.h"
#include "mon_stats.h"

struct ArmourBase;
struct Weapon;
struct Symbol;

/**
 * Struct containing data about the type of a monster (not an individual monster)
 *
 * e.g. human, ghoul, night elf, etc.
 */
struct MonType
{
    char               id[5];
    char               name[32];
    char               desc[32];
    struct Symbol*     symbol;
    struct ArmourBase* base_armour; // All creatures have a base armour for if they don't have equipment
    struct WeaponBase* base_weapon;
    struct Stats       base_stats;
    int                vision_radius;
    MonAttrMoveFlags   move_flags;
};

struct MonType* mon_type_look_up_by_id(const char* id);

extern struct MonType* g_mon_type;
extern int g_mon_type_count;

#endif
