#ifndef YUN_SPELL_SKEWER_H
#define YUN_SPELL_SKEWER_H

#include "list.h"

#include <stdbool.h>

struct MapLocation;
struct Mon;
struct Spell;

void spell_cast_skewer(struct Spell* spell, struct Mon* caster);

#endif
