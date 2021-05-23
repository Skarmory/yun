#include "spell_effect.h"

#include "monster.h"

#include <stddef.h>

struct SpellEffect* g_spell_effects = NULL;
int g_spell_effects_count = 0;

static void _spell_effect_affect_mon(const struct SpellEffect* effect, const struct SpellEffectArgs* args)
{
    switch(effect->effect_type)
    {
        case SPELL_EFFECT_TYPE_DAMAGE_HEALTH:
        {
            ListNode* n = NULL;
            list_for_each(args->affected_mons, n)
            {
                struct Mon* mon = n->data;
                mon->stats.hp -= effect->amount;
                mon_chk_dead(mon);
            }
            break;
        }
    }
}

void spell_effect_execute(const struct SpellEffect* effect, const struct SpellEffectArgs* args)
{
    switch(effect->event)
    {
        case SPELL_EVENT_HIT_MON:
        {
            _spell_effect_affect_mon(effect, args);
            break;
        }
    }
}
