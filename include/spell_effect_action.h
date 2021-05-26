#ifndef YUN_SPELL_EFFECT_ACTION_H
#define YUN_SPELL_EFFECT_ACTION_H

enum SpellEffectAction
{
    SPELL_EFFECT_ACTION_DAMAGE_HEALTH
};

struct SpellEffectAction_CauseDamage
{
    int dice_count;
    int dice_sides;
};

#endif
