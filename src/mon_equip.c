#include "mon_equip.h"

#include "log.h"
#include "object.h"
#include "obj_armour.h"
#include "obj_weapon.h"

#include <stdlib.h>

struct Equipment* new_equipment(void)
{
    struct Equipment* equipment = (struct Equipment*) malloc(sizeof(struct Equipment));
    equipment->head = NULL;
    equipment->shoulders = NULL;
    equipment->chest = NULL;
    equipment->hands = NULL;
    equipment->legs = NULL;
    equipment->feet = NULL;
    equipment->main_hand = NULL;
    equipment->off_hand = NULL;

    return equipment; 
}

void free_equipment(struct Equipment* equipment)
{
    free(equipment);
}

bool equipment_is_equipped(struct Equipment* equipment, struct Object* obj)
{
    switch(obj->objtype)
    {
        case OBJ_TYPE_WEAPON:
        {
            return (equipment->main_hand == obj->objtype_ptr.weapon || equipment->off_hand == obj);
        }

        case OBJ_TYPE_ARMOUR:
        {
            return (equipment->head == obj->objtype_ptr.armour      ||
                    equipment->shoulders == obj->objtype_ptr.armour ||
                    equipment->chest == obj->objtype_ptr.armour     ||
                    equipment->hands == obj->objtype_ptr.armour     ||
                    equipment->feet == obj->objtype_ptr.armour      ||
                    equipment->legs == obj->objtype_ptr.armour);
        }
    }

    return false;
}

bool equipment_slot_free(struct Equipment* equipment, enum EquipmentSlot slot)
{
    switch(slot)
    {
        case EQUIP_SLOT_HEAD:      return equipment->head == NULL;
        case EQUIP_SLOT_SHOULDERS: return equipment->shoulders == NULL;
        case EQUIP_SLOT_CHEST:     return equipment->chest == NULL;
        case EQUIP_SLOT_HANDS:     return equipment->hands == NULL;
        case EQUIP_SLOT_LEGS:      return equipment->legs == NULL;
        case EQUIP_SLOT_FEET:      return equipment->feet == NULL;
        case EQUIP_SLOT_MAIN_HAND: return equipment->main_hand == NULL;
        case EQUIP_SLOT_OFF_HAND:  return equipment->off_hand == NULL;
        case EQUIP_SLOT_MAX:       return false;
    }
    return false;
}

bool equipment_equip_obj(struct Equipment* equipment, struct Object* obj, enum EquipmentSlot slot)
{
    if(obj_is_equipment(obj))
    {
        log_format_msg(DEBUG, "slot: %d", slot);
        switch(slot)
        {
            case EQUIP_SLOT_MAIN_HAND:
            {
                equipment->main_hand = obj->objtype_ptr.weapon;
                return true;
            }

            case EQUIP_SLOT_OFF_HAND:
            {
                equipment->off_hand = obj;
                return true;
            }

            case EQUIP_SLOT_HEAD:
            {
                equipment->head = obj->objtype_ptr.armour;
                return true;
            }

            case EQUIP_SLOT_SHOULDERS:
            {
                equipment->shoulders = obj->objtype_ptr.armour;
                return true;
            }

            case EQUIP_SLOT_CHEST:
            {
                equipment->chest = obj->objtype_ptr.armour;
                return true;
            }

            case EQUIP_SLOT_HANDS:
            {
                equipment->hands = obj->objtype_ptr.armour;
                return true;
            }

            case EQUIP_SLOT_LEGS:
            {
                equipment->legs = obj->objtype_ptr.armour;
                return true;
            }

            case EQUIP_SLOT_FEET:
            {
                equipment->feet = obj->objtype_ptr.armour;
                return true;
            }

            case EQUIP_SLOT_MAX:
                return false;
        }
    }

    return false;
}

struct Object* equipment_unequip_obj(struct Equipment* equipment, struct Object* obj)
{
    if(obj->objtype == OBJ_TYPE_WEAPON)
    {
        if(obj->objtype_ptr.weapon == equipment->main_hand)
        {
            equipment->main_hand = NULL;
            return obj;
        }

        if(obj == equipment->off_hand)
        {
            equipment->off_hand = NULL;
            return obj;
        }
    }

    if(obj->objtype == OBJ_TYPE_ARMOUR)
    {
        if(obj->objtype_ptr.armour == equipment->head)
        {
            equipment->head = NULL;
            return obj;
        }

        if(obj->objtype_ptr.armour == equipment->shoulders)
        {
            equipment->shoulders = NULL;
            return obj;
        }

        if(obj->objtype_ptr.armour == equipment->chest)
        {
            equipment->chest = NULL;
            return obj;
        }

        if(obj->objtype_ptr.armour == equipment->hands)
        {
            equipment->hands = NULL;
            return obj;
        }

        if(obj->objtype_ptr.armour == equipment->legs)
        {
            equipment->legs = NULL;
            return obj;
        }

        if(obj->objtype_ptr.armour == equipment->feet)
        {
            equipment->feet = NULL;
            return obj;
        }
    }

    return NULL;
}

struct Object* equipment_unequip_slot(struct Equipment* equipment, enum EquipmentSlot slot)
{
    struct Object* unequipped = NULL;
    switch(slot)
    {
        case EQUIP_SLOT_MAIN_HAND:
            if(equipment->main_hand)
            {
                unequipped = equipment->main_hand->obj;
                equipment->main_hand = NULL;
            }
            break;

        case EQUIP_SLOT_OFF_HAND:
            if(equipment->off_hand)
            {
                unequipped = equipment->off_hand;
                equipment->off_hand = NULL;
            }
            break;

        case EQUIP_SLOT_HEAD:
            if(equipment->head)
            {
                unequipped = equipment->head->obj;
                equipment->head = NULL;
            }
            break;

        case EQUIP_SLOT_SHOULDERS:
            if(equipment->shoulders)
            {
                unequipped = equipment->shoulders->obj;
                equipment->shoulders = NULL;
            }
            break;

        case EQUIP_SLOT_CHEST:
            if(equipment->chest)
            {
                unequipped = equipment->chest->obj;
                equipment->chest = NULL;
            }
            break;

        case EQUIP_SLOT_HANDS:
            if(equipment->hands)
            {
                unequipped = equipment->hands->obj;
                equipment->hands = NULL;
            }
            break;

        case EQUIP_SLOT_LEGS:
            if(equipment->legs)
            {
                unequipped = equipment->legs->obj;
                equipment->legs = NULL;
            }
            break;

        case EQUIP_SLOT_FEET:
            if(equipment->feet)
            {
                unequipped = equipment->feet->obj;
                equipment->feet = NULL;
            }
            break;

        case EQUIP_SLOT_MAX:
            break;
    }

    return unequipped;
}

enum EquipmentSlot equipment_slot_by_obj(struct Equipment* equipment, struct Object* obj)
{
    if(obj->objtype == OBJ_TYPE_WEAPON)
    {
        if(obj->objtype_ptr.weapon == equipment->main_hand)
            return EQUIP_SLOT_MAIN_HAND;

        if(obj == equipment->off_hand)
            return EQUIP_SLOT_OFF_HAND;
    }

    if(obj->objtype == OBJ_TYPE_ARMOUR)
    {
        if(obj->objtype_ptr.armour == equipment->head)
            return EQUIP_SLOT_HEAD;

        if(obj->objtype_ptr.armour == equipment->shoulders)
            return EQUIP_SLOT_SHOULDERS;

        if(obj->objtype_ptr.armour == equipment->chest)
            return EQUIP_SLOT_CHEST;

        if(obj->objtype_ptr.armour == equipment->hands)
            return EQUIP_SLOT_HANDS;

        if(obj->objtype_ptr.armour == equipment->legs)
            return EQUIP_SLOT_LEGS;

        if(obj->objtype_ptr.armour == equipment->feet)
            return EQUIP_SLOT_FEET;
    }

    return EQUIP_SLOT_MAX;
}

int equipment_armour_total(struct Equipment* equipment)
{
    int armour = 0;

    armour += (equipment->head ? equipment->head->armour_value : 0);
    armour += (equipment->shoulders ? equipment->shoulders->armour_value : 0);
    armour += (equipment->chest ? equipment->chest->armour_value : 0);
    armour += (equipment->hands ? equipment->hands->armour_value : 0);
    armour += (equipment->legs ? equipment->legs->armour_value : 0);
    armour += (equipment->feet ? equipment->feet->armour_value : 0);

    return armour;
}
