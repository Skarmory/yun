#include "armour.h"

#include <stdlib.h>

#include "object.h"

#define ARMOUR(str, agi, in, spi, sta)\
    { str, agi, in, spi, sta, NULL }

struct Armour* new_armour(int armour_type)
{
    struct Armour* armour = (struct Armour*) malloc(sizeof(struct Armour));

    armour->strength = armours[armour_type].strength;
    armour->agility = armours[armour_type].agility;
    armour->intelligence = armours[armour_type].intelligence;
    armour->spirit = armours[armour_type].spirit;
    armour->stamina = armours[armour_type].stamina;

    armour->obj = (struct Object*) malloc(sizeof(struct Object));
    armour->obj->name = armour_names[armour_type];
    armour->obj->objtype_ptr.armour = armour;
    armour->obj->objtype = ARMOUR;
    list_entry_init(&armour->obj->obj_list_entry);

    return armour;
}

void free_armour(struct Armour* armour)
{
    free(armour->obj);
    free(armour);
}

struct Armour armours[] = 
{
    ARMOUR(0, 0, 0, 0, 0)
};

char* armour_names[] = {
    "Unarmoured"
};
