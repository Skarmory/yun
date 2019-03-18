#include "equip.h"

#include "weapon.h"
#include "object.h"

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

bool equipment_equip_obj(struct Equipment* equipment, struct Object* obj, enum EquipmentSlot slot)
{
    switch(obj->objtype)
    {
        case WEAPON:
        {
            switch(slot)
            {
                case MAIN_HAND:
                {
                    equipment->main_hand = obj->objtype_ptr.weapon;
                    return true;
                }
                case OFF_HAND:
                {
                    equipment->off_hand = obj->objtype_ptr.weapon;
                    return true;
                }
            }
        }
        break;

        case ARMOUR:
            break;
    }
    return false;
}

struct Object* equipment_unequip_obj(struct Equipment* equipment, enum EquipmentSlot slot)
{
    struct Object* unequipped = NULL;
    switch(slot)
    {
        case MAIN_HAND:
            if(equipment->main_hand)
            {
                unequipped = equipment->main_hand->obj;
                equipment->main_hand = NULL;
            }
            break;
        case OFF_HAND:
            if(equipment->off_hand)
            {
                unequipped = equipment->off_hand->obj;
                equipment->off_hand = NULL;
            }
            break;
    }

    return unequipped;
}
