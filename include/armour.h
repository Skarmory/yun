#ifndef ARMOUR_H
#define ARMOUR_H

#include "defs.h"

struct Object;

#define A_UNARMOURED 0

struct Armour
{
    byte strength;
    byte agility;
    byte intelligence;
    byte spirit;
    byte stamina;

    struct Object* obj;
};

struct Armour* new_armour(int armour_type);
void free_armour(struct Armour* armour);

extern struct Armour armours[];
extern char* armour_names[];
extern char* armour_descs[];

#endif
