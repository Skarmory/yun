#include "pathing_flags.h"

#include <string.h>

enum PathingFlag pathing_path_flag_from_string(const char* name)
{
    if(strcmp(name, "none") == 0 || strcmp(name, "NONE") == 0)
    {
        return PATHING_NONE;
    }

    if(strcmp(name, "ground") == 0 || strcmp(name, "GROUND") == 0)
    {
        return PATHING_GROUND;
    }

    if(strcmp(name, "water") == 0 || strcmp(name, "WATER") == 0)
    {
        return PATHING_WATER;
    }

    if(strcmp(name, "flying") == 0 || strcmp(name, "FLYING") == 0)
    {
        return PATHING_FLYING;
    }

    return PATHING_NONE;
}

