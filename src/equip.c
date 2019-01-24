#include "equip.h"

#include "weapon.h"

#include <stdlib.h>

struct Equipment* new_equipment(void)
{
    struct Equipment* equipment = (struct Equipment*) malloc(sizeof(struct Equipment));
    equipment->main_hand = NULL;
    equipment->off_hand = NULL;

    return equipment; 
}

void free_equipment(struct Equipment* equipment)
{
    if(equipment->main_hand)
        free_weapon(equipment->main_hand);

    if(equipment->off_hand)
        free_weapon(equipment->off_hand);

    free(equipment);
}
