#include "spell.h"

#include "colour.h"
#include "gameplay_commands.h"
#include "geom.h"
#include "log.h"
#include "look.h"
#include "map.h"
#include "map_cell.h"
#include "message.h"
#include "monster.h"
#include "mon_type.h"
#include "player.h"
#include "spell_effect.h"
#include "spell_skewer.h"
#include "term.h"

#include <string.h>

struct Spell* g_spells = NULL;
int g_spells_count = 0;

void spell_cast(struct Spell* spell)
{
    struct Mon* caster = g_you->mon; // TODO: Update for monster spellcasting

    switch(spell->spatial_type)
    {
        case SPELL_SPATIAL_SKEWER:
        {
            spell_cast_skewer(spell, caster);
            break;
        }

        default:
        {
            log_msg(LOG_DEBUG, "Unknown spell spatial type");
            break;
        }
    }
}

struct Spell* spell_look_up_by_id(const char* id)
{
    for(int idx = 0; idx < g_spells_count; ++idx)
    {
        if(strcmp(g_spells[idx].id, id) == 0)
        {
            return &g_spells[idx];
        }
    }

    return NULL;
}
