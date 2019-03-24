#ifndef NAXX_ARMOUR_H
#define NAXX_ARMOUR_H

#include "defs.h"
#include "equip.h"

struct Object;

#define A_UNARMOURED 0
#define A_MAGISTERS_CROWN 1
#define A_MAGISTERS_MANTLE 2
#define A_MAGISTERS_ROBE 3
#define A_MAGISTERS_GLOVES 4
#define A_MAGISTERS_LEGGINGS 5
#define A_MAGISTERS_BOOTS 6
#define A_SHADOWCRAFT_CAP 7
#define A_SHADOWCRAFT_SPAULDERS 8
#define A_SHADOWCRAFT_TUNIC 9
#define A_SHADOWCRAFT_GLOVES 10
#define A_SHADOWCRAFT_PANTS 11
#define A_SHADOWCRAFT_BOOTS 12
#define A_BEASTSTALKERS_CAP 13
#define A_BEASTSTALKERS_SPAULDERS 14
#define A_BEASTSTALKERS_TUNIC 15
#define A_BEASTSTALKERS_GLOVES 16
#define A_BEASTSTALKERS_PANTS 17
#define A_BEASTSTALKERS_BOOTS 18
#define A_VALOR_HELM 19
#define A_VALOR_SPAULDERS 20
#define A_VALOR_BREASTPLATE 21
#define A_VALOR_GAUNTLETS 22
#define A_VALOR_LEGPLATES 23
#define A_VALOR_BOOTS 24

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
