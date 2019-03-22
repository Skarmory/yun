#ifndef NAXX_EQUIP_H
#define NAXX_EQUIP_H

#include <stdbool.h>

struct Weapon;
struct Armour;
struct Object;

enum EquipmentSlot
{
    EQUIP_SLOT_HEAD,
    EQUIP_SLOT_SHOULDERS,
    EQUIP_SLOT_CHEST,
    EQUIP_SLOT_HANDS,
    EQUIP_SLOT_LEGS,
    EQUIP_SLOT_FEET,
    EQUIP_SLOT_MAIN_HAND,
    EQUIP_SLOT_OFF_HAND,
    EQUIP_SLOT_MAX
};

struct Equipment
{
    struct Armour* head;
    struct Armour* shoulders;
    struct Armour* chest;
    struct Armour* hands;
    struct Armour* legs;
    struct Armour* feet;
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
