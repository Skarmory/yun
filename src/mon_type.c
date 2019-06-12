#include "mon_type.h"

#include "colour.h"
#include "log.h"
#include "mon_attr.h"
#include "mon_stats.h"
#include "obj_armour.h"
#include "obj_weapon.h"
#include "symbol.h"

#include <stddef.h>
#include <string.h>

/**
 * Do all runtime initialisation for montypes
 */
void init_montypes(void)
{
    for(unsigned int idx = 0; idx < g_mon_type_count; idx++)
    {
        // Create base armour and weapons for each mon type
        g_mon_type[idx].base_armour = new_armour(g_mon_type[idx].base_armour_idx);
        g_mon_type[idx].base_weapon = new_weapon(g_mon_type[idx].base_weapon_idx);
        g_mon_type[idx].symbol->bg = *COL(CLR_DEFAULT);
    }
}

void uninit_montypes(void)
{
    for(unsigned int idx = 0; idx < g_mon_type_count; idx++)
    {
        // Create base armour and weapons for each mon type
        free_armour(g_mon_type[idx].base_armour);
        free_weapon(g_mon_type[idx].base_weapon);
    }
}

struct MonType* mon_type_lookup_by_name(const char* name)
{
    for(int idx = 0; idx < g_mon_type_count; ++idx)
    {
        if(strcmp(g_mon_type[idx].name, name) == 0)
            return &g_mon_type[idx];
    }

    return NULL;
}
