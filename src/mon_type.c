#include "mon_type.h"

#include "obj_armour.h"
#include "obj_weapon.h"

#include <scieppend/core/colour.h>
#include <scieppend/core/log.h>
#include <scieppend/core/symbol.h>

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

struct MonType* g_mon_type = NULL;
int g_mon_type_count = 0;

struct MonType* mon_type_look_up_by_id(const char* id)
{
    for(int idx = 0; idx < g_mon_type_count; ++idx)
    {
        if(strcmp(g_mon_type[idx].id, id) == 0)
        {
            return &g_mon_type[idx];
        }
    }

    return NULL;
}
