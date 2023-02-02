#include "obj_armour.h"

#include "object.h"

#include <scieppend/core/log.h>
#include <scieppend/core/symbol.h>

#include <stdlib.h>
#include <string.h>

struct ArmourBase* g_armour_base = NULL;
int g_armour_base_count = 0;

struct Armour* armour_new(struct ArmourBase* base)
{
    struct Armour* armour = malloc(sizeof(struct Armour));
    armour->base = base;
    armour->armour_value = base->armour_value;

    armour->obj = malloc(sizeof(struct Object));
    armour->obj->name = base->name;
    armour->obj->desc = base->desc;

    switch(base->class)
    {
        case ARMOUR_CLASS_NATURAL:
        {
            break;
        }
        case ARMOUR_CLASS_NONE:
        {
            log_msg(LOG_DEBUG, "Armour created with ARMOUR_CLASS_NONE");
            break;
        }
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

struct ArmourBase* armour_base_look_up_by_id(const char* id)
{
    for(int idx = 0; idx < g_armour_base_count; ++idx)
    {
        if(strcmp(id, g_armour_base[idx].id) == 0)
        {
            return &g_armour_base[idx];
        }
    }

    return NULL;
}

enum ArmourClass armour_class_from_string(const char* name)
{
    if(strcmp(name, "natural") == 0) return ARMOUR_CLASS_NATURAL;

    return ARMOUR_CLASS_NONE;
}
