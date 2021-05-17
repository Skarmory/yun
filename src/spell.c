#include "spell.h"

#include <string.h>

struct Spell* g_spell = NULL;
int g_spell_count = 0;

struct Spell* spell_look_up_by_id(const char* id)
{
    for(int idx = 0; idx < g_spell_count; ++idx)
    {
        if(strcmp(g_spell[idx].id, id) == 0)
        {
            return &g_spell[idx];
        }
    }

    return NULL;
}
