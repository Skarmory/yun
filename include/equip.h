#ifndef EQUIP_H
#define EQUIP_H

#include <stdbool.h>

struct Weapon;
struct Armour;
struct Object;

enum EquipmentSlot
{
    EQUIP_SLOT_MAIN_HAND,
    EQUIP_SLOT_OFF_HAND,
    EQUIP_SLOT_MAX
};

struct Equipment
{
    struct Weapon* main_hand;
    struct Weapon* off_hand;  // This will be null unless dual wielding
};

struct Equipment* new_equipment(void);
void free_equipment(struct Equipment* equipment);

bool equipment_is_equipped(struct Equipment* equipment, struct Object* obj);
bool equipment_equip_obj(struct Equipment* equipment, struct Object* obj, enum EquipmentSlot slot);
struct Object* equipment_unequip_obj(struct Equipment* equipment, struct Object* obj);
struct Object* equipment_unequip_slot(struct Equipment* equipment, enum EquipmentSlot slot);
enum EquipmentSlot equipment_slot_by_obj(struct Equipment* equipment, struct Object* obj);

#endif
