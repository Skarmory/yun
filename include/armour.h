#ifndef ARMOUR_H
#define ARMOUR_H

#include "defs.h"

#define A_UNARMOURED 0

struct Armour
{
    char* name;
    byte strength;
    byte agility;
    byte intelligence;
    byte spirit;
    byte stamina;
};

extern struct Armour armours[];

#endif
