#ifndef YUN_FEATURE_H
#define YUN_FEATURE_H

#include "pathing_flags.h"

#include <stdbool.h>

struct Symbol;

struct Feature
{
    int id_hash;
    char id[5];
    char name[32];
    char desc[32];
    struct Symbol* symbol;
    PathingFlags pathing_flags;
    bool block_sight;
};

const struct Feature* feature_look_up_by_id(const char* id);
const struct Feature* feature_look_up_by_id_hash(int id_hash);

extern struct Feature* g_features;
extern int g_features_count;

#endif
