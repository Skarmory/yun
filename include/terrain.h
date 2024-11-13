#ifndef YUN_TERRAIN_H
#define YUN_TERRAIN_H

#include "pathing_flags.h"

struct Parser;

struct Terrain
{
    int id_hash;
    char id[5];
    char name[32];
    char desc[32];
    struct Symbol* symbol;
    PathingFlags pathing_flags;
};

const struct Terrain* terrain_look_up_by_id(const char* id);

void terrain_parsing_finalise(struct Parser* parser);

#endif

