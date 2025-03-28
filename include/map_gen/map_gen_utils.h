#ifndef YUN_MAPGEN_UTILS_H
#define YUN_MAPGEN_UTILS_H

#include <stdbool.h>

struct List;
struct MapGenCell;
struct MapGenLocation;

void mapgen_util_enlist_orthogonals_random_order(struct MapGenCell* cell, struct MapGenLocation* loc, struct List* list);
bool mapgen_util_is_border_loc(struct MapGenCell* cell, struct MapGenLocation* loc);
bool mapgen_util_is_corner_wall(struct MapGenLocation* loc);
bool mapgen_util_is_corridor(struct MapGenLocation* loc);
bool mapgen_util_is_horizontal_wall(struct MapGenLocation* loc);
bool mapgen_util_is_solid_rock(struct MapGenLocation* loc);
bool mapgen_util_is_vertical_wall(struct MapGenLocation* loc);
bool mapgen_util_is_wall(struct MapGenLocation* loc);

#endif
