#include "obj_armour.h"

#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "object.h"
#include "symbol.h"

struct ArmourBase* g_armour_base = NULL;
int g_armour_base_count = 0;

struct Armour* armour_new(struct ArmourBase* base)
{
    struct Armour* armour = malloc(sizeof(struct Armour));

    armour->armour_value = base->armour_value;

    armour->obj = malloc(sizeof(struct Object));
    armour->obj->name = base->name;
    armour->obj->desc = base->desc;

    switch(base->class)
    {
        case ARMOUR_CLASS_CLOTH:
            armour->obj->symbol = &g_symbol_armour_cloth;
            break;
        case ARMOUR_CLASS_LEATHER:
            armour->obj->symbol = &g_symbol_armour_leather;
            break;
        case ARMOUR_CLASS_MAIL:
            armour->obj->symbol = &g_symbol_armour_mail;
            break;
        case ARMOUR_CLASS_PLATE:
            armour->obj->symbol = &g_symbol_armour_plate;
            break;
        case ARMOUR_CLASS_NONE:
            log_msg(DEBUG, "Armour created with ARMOUR_CLASS_NONE");
            break;
    }

    armour->obj->objtype_ptr.armour = armour;
    armour->obj->objtype = OBJ_TYPE_ARMOUR;

    return armour;
}

void armour_free(struct Armour* armour)
{
    free(armour->obj);
    free(armour);
}

struct ArmourBase* armour_base_lookup_by_name(const char* name)
{
    for(int idx = 0; idx < g_armour_base_count; ++idx)
    {
        if(strcmp(name, g_armour_base[idx].name) == 0)
            return &g_armour_base[idx];
    }

    return NULL;
}

enum ArmourClass armour_class_from_string(const char* name)
{
    if(strcmp(name, "cloth") == 0)   return ARMOUR_CLASS_CLOTH;
    if(strcmp(name, "leather") == 0) return ARMOUR_CLASS_LEATHER;
    if(strcmp(name, "mail") == 0)    return ARMOUR_CLASS_MAIL;
    if(strcmp(name, "plate") == 0)   return ARMOUR_CLASS_PLATE;

    return ARMOUR_CLASS_NONE;
}
