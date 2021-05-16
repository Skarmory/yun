#include "map_gen_utils.h"

#include "map_location.h"

#include <stddef.h>

void map_gen_util_make_corridor(struct MapLocation* loc)
{
    loc->symbol.sym = '#';
    loc->pathing_flags = PATHING_GROUND;
    loc->feature = NULL;
}

void map_gen_util_make_floor(struct MapLocation* loc)
{
    loc->symbol.sym = '.';
    loc->pathing_flags = PATHING_GROUND;
    loc->feature = NULL;
}
