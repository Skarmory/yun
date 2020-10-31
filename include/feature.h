#ifndef YUN_FEATURE_H
#define YUN_FEATURE_H

#include "pathing_flags.h"

#include <stdbool.h>

struct Symbol;

struct Feature
{
    char id[5];
    char name[32];
    char desc[32];
    struct Symbol* symbol;
    PathingFlags pathing_flags;
    bool block_sight;
};

struct Feature* feature_look_up_by_id(const char* id);

extern struct Feature* g_features;
extern int g_features_count;

#endif
