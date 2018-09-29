#include "equip.h"

#include "weapon.h"

#include <stdlib.h>

struct Equipment* new_equipment(void)
{
    struct Equipment* equipment = (struct Equipment*) malloc(sizeof(struct Equipment));

    return equipment; 
}

void free_equipment(struct Equipment* equipment)
{
    free_weapon(equipment->main_hand);
    free_weapon(equipment->off_hand); 
}
