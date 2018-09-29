#ifndef MONTYPE_H
#define MONTYPE_H

struct Weapon;

#define MT_GHOUL 0
#define MT_PLAYER 1

/**
 * Struct containing data about the type of a monster (not an individual monster)
 *
 * e.g. human, ghoul, night elf, etc.
 */
struct MonType
{
    char* name; // Name of this type of mon
    char sym; // ascii symbol of this mon
    signed char fg; // foreground colour
    signed char bg; // background colour
    int attr; // symbol attributes
    struct Weapon* base_weapon;
    int strength, agility, intelligence, spirit, stamina;
    int pathing;
};

extern struct MonType mon_type[];

#endif
