#ifndef MONTYPE_H
#define MONTYPE_H

struct Armour;
struct Weapon;
struct Symbol;

#define MT_GHOUL    0
#define MT_HUMAN    1
#define MT_DWARF    2
#define MT_NELF     3
#define MT_GNOME    4
#define MT_ORC      5
#define MT_FORSAKEN 6
#define MT_TAUREN   7
#define MT_TROLL    8

#define MT_RANGE_START 0
#define MT_RANGE_END   9

/**
 * Struct containing data about the type of a monster (not an individual monster)
 *
 * e.g. human, ghoul, night elf, etc.
 */
struct MonType
{
    char*                name; // Name of this type of mon
    struct Symbol*       symbol;
    int                  base_armour_idx;
    int                  base_weapon_idx;
    struct Armour*       base_armour; // All creatures have a base armour for if they don't have equipment
    struct Weapon*       base_weapon;
    int                  strength, agility, intelligence, spirit, stamina;
    int                  pathing;
};

void init_montypes(void);
void uninit_montypes(void);

extern struct MonType mon_type[];

#endif
