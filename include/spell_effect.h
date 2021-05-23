#ifndef YUN_SPELL_EFFECT_H
#define YUN_SPELL_EFFECT_H

#include "list.h"

enum SpellEvent
{
    SPELL_EVENT_HIT_MON
};

enum SpellEffectType
{
    SPELL_EFFECT_TYPE_DAMAGE_HEALTH
};

struct SpellEffect
{
    enum SpellEvent event;
    enum SpellEffectType effect_type;
    int amount;
};

struct SpellEffectArgs
{
    List* affected_locations;
    List* affected_mons;
};

void spell_effect_execute(const struct SpellEffect* effect, const struct SpellEffectArgs* args);

extern struct SpellEffect* g_spell_effects;
extern int g_spell_effects_count;

#endif
