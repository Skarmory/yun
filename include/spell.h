#ifndef YUN_SPELL_H
#define YUN_SPELL_H

#include "list.h"

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
    enum SpellSpatialType spatial_type;
    List spell_effects;
};

void spell_cast(struct Spell* spell);

struct Spell* spell_look_up_by_id(const char* id);

extern struct Spell* g_spells;
extern int g_spells_count;

#endif
