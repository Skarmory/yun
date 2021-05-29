#ifndef YUN_SPELL_EFFECT_H
#define YUN_SPELL_EFFECT_H

#include "list.h"
#include "spell_effect_action.h"

enum SpellEffectEvent
{
    SPELL_EFFECT_EVENT_HIT_MON
};

struct SpellEffect
{
    char                   id[5];
    enum SpellEffectEvent  event;
    enum SpellEffectAction action;
    void*                  action_data;
};

/*
 * Contains the things affected by a Spell.
 * A spell effect can grab what it's interested in from these and apply its effects.
 * affected_locations: List of MapLocation
 * affected_mons: List of Mon
 */
struct SpellEffectArgs
{
    List* affected_locations;
    List* affected_mons;
};

void spell_effect_execute(struct SpellEffect* effect, struct SpellEffectArgs* args);

struct SpellEffect* spell_effect_look_up_by_id(const char* id);

extern struct SpellEffect* g_spell_effects;
extern int g_spell_effects_count;

#endif
