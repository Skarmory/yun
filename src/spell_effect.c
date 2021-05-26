#include "spell_effect.h"
#include "spell_effect_action.h"

#include "monster.h"

#include <stddef.h>
#include <string.h>

struct SpellEffect* g_spell_effects = NULL;
int g_spell_effects_count = 0;

static void _spell_effect_affect_mon(const struct SpellEffect* effect, const struct SpellEffectArgs* args)
{
    switch(effect->action)
    {
        case SPELL_EFFECT_ACTION_DAMAGE_HEALTH:
        {
            struct SpellEffectAction_CauseDamage* data = effect->action_data;

            ListNode* n = NULL;
            list_for_each(args->affected_mons, n)
            {
                struct Mon* mon = n->data;
                mon->stats.hp -= roll_dice(data->dice_count, data->dice_sides);
                mon_chk_dead(mon);
            }
            break;
        }
    }
}

/* ---------- SECTION EXTERNALLY VISIBLE ---------- */

void spell_effect_execute(const struct SpellEffect* effect, const struct SpellEffectArgs* args)
{
    switch(effect->event)
    {
        case SPELL_EFFECT_EVENT_HIT_MON:
        {
            _spell_effect_affect_mon(effect, args);
            break;
        }
    }
}

const struct SpellEffect* spell_effect_look_up_by_id(const char* id)
{
    for(int idx = 0; idx < g_spell_effects_count; ++idx)
    {
        if(strcmp(g_spell_effects[idx].id, id) == 0)
        {
            return &g_spell_effects[idx];
        }
    }

    return NULL;
}
