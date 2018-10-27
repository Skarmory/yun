#include "armour.h"

#include <stdlib.h>

#include "object.h"

#define ARMOUR(n, str, agi, in, spi, sta)\
    { n, str, agi, in, spi, sta }

struct Armour* new_armour(int armour_type)
{
    struct Armour* armour = (struct Armour*) malloc(sizeof(struct Armour));

    armour->name = armours[armour_type].name;
    armour->strength = armours[armour_type].strength;
    armour->agility = armours[armour_type].agility;
    armour->intelligence = armours[armour_type].intelligence;
    armour->spirit = armours[armour_type].spirit;
    armour->stamina = armours[armour_type].stamina;

    armour->obj = (struct Object*) malloc(sizeof(struct Object));
    armour->obj->objtype_ptr.armour = armour;
    armour->obj->objtype = ARMOUR;
    armour->obj->next = NULL;

    return armour;
}

void free_armour(struct Armour* armour)
{
    free(armour->obj);
    free(armour);
}

struct Armour armours[] = 
{
    ARMOUR("Unarmoured", 0, 0, 0, 0, 0)
};
