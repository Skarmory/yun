#ifndef NAXX_ARMOUR_H
#define NAXX_ARMOUR_H

#include "defs.h"
#include "mon_equip.h"

struct Object;

enum ArmourClass
{
    ARMOUR_CLASS_CLOTH,
    ARMOUR_CLASS_LEATHER,
    ARMOUR_CLASS_MAIL,
    ARMOUR_CLASS_PLATE,
    ARMOUR_CLASS_NATURAL,
    ARMOUR_CLASS_NONE
};

struct ArmourBase
{
    char id[5];
    char name[32];
    char desc[256];
    enum EquipmentSlot slot;
    enum ArmourClass class;
    int armour_value;
};

struct Armour
{
    int armour_value;

    struct ArmourBase* base;
    struct Object*     obj;
};

struct Armour* armour_new(struct ArmourBase* base);
void armour_free(struct Armour* armour);

struct ArmourBase* armour_base_look_up_by_id(const char* id);
enum ArmourClass armour_class_from_string(const char* name);

extern struct ArmourBase* g_armour_base;
extern int g_armour_base_count;

#endif
