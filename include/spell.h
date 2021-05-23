#ifndef YUN_SPELL_H
#define YUN_SPELL_H

struct SpellEffect;

enum SpellSpatialType
{
    SPELL_SPATIAL_INSTANTANEOUS,
    SPELL_SPATIAL_SKEWER,
    SPELL_SPATIAL_MISSILE,
    SPELL_SPATIAL_AREA_SQUARE
};

struct Spell
{
    char id[5];
    int cost;
    enum SpellSpatialType spatial_type;
    struct SpellEffect* effect;
};

void spell_cast(const struct Spell* spell);

struct Spell* spell_look_up_by_id(const char* id);

extern struct Spell* g_spell;
extern int g_spell_count;

#endif
