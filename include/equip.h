#ifndef EQUIP_H
#define EQUIP_H

#include <stdbool.h>

struct Weapon;
struct Armour;
struct Object;

enum EquipmentSlot
{
    MAIN_HAND,
    OFF_HAND
};

struct Equipment
{
    struct Weapon* main_hand;
    struct Weapon* off_hand;  // This will be null unless dual wielding
};

struct Equipment* new_equipment(void);
void free_equipment(struct Equipment* equipment);

bool equipment_equip_obj(struct Equipment* equipment, struct Object* obj, enum EquipmentSlot slot);
struct Object* equipment_unequip_obj(struct Equipment* equipment, enum EquipmentSlot slot);

#endif
