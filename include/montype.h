#ifndef MONTYPE_H
#define MONTYPE_H

#include "attack_data.h"

#define MT_GHOUL 0
#define MT_PLAYER 1

struct MonType
{
    char* name; // Name of this type of mon
    char sym; // ascii symbol of this mon
    signed char fg; // foreground colour
    signed char bg; // background colour
    int attr; // symbol attributes
    struct Weapon* base_weapon;
    int num_hitdice;
    int sides_per_hitdie;
    int strength, agility, intelligence, spirit, stamina;
};

extern struct MonType mon_type[];

#endif
