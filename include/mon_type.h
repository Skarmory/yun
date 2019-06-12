#ifndef NAXX_MONTYPE_H
#define NAXX_MONTYPE_H

struct Armour;
struct Weapon;
struct Symbol;

/**
 * Struct containing data about the type of a monster (not an individual monster)
 *
 * e.g. human, ghoul, night elf, etc.
 */
struct MonType
{
    char*                name; // Name of this type of mon
    char*                desc;
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

struct MonType* mon_type_lookup_by_name(const char* name);

struct MonType* g_mon_type;
int g_mon_type_count;

#endif
