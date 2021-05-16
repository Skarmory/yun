#ifndef YUN_MAP_GEN_UTILS
#define YUN_MAP_GEN_UTILS

struct MapLocation;

/* Turn a map location into a corridor */
void map_gen_util_make_corridor(struct MapLocation* loc);
void map_gen_util_make_floor(struct MapLocation* loc);

#endif

