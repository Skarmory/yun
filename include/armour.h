#ifndef NAXX_ARMOUR_H
#define NAXX_ARMOUR_H

#include "defs.h"
#include "equip.h"

struct Object;

#define A_UNARMOURED 0

struct Armour
{
    int armour_value;
    enum EquipmentSlot slot;
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
