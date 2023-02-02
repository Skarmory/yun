#ifndef YUN_PATHING_FLAGS_H
#define YUN_PATHING_FLAGS_H

#include <scieppend/core/bit_flags.h>

enum PathingFlag
{
    PATHING_NONE   = 0u,
    PATHING_GROUND = BIT_FLAG(1),
    PATHING_WATER  = BIT_FLAG(2),
    PATHING_FLYING = BIT_FLAG(3)
};

typedef unsigned int PathingFlags;

enum PathingFlag pathing_path_flag_from_string(const char* name);

#endif
