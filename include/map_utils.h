#ifndef YUN_MAP_UTILS_H
#define YUN_MAP_UTILS_H

#include <stdbool.h>

struct List;
struct MapCell;
struct MapLocation;

void map_util_enlist_orthogonals_random_order(struct MapCell* cell, struct MapLocation* loc, struct List* list);
bool map_util_is_border_loc(struct MapCell* cell, struct MapLocation* loc);
//bool map_util_is_corner_wall(struct MapLocation* loc);
bool map_util_is_terrain_type(const struct MapLocation* loc, int terrain_id);
bool map_util_is_feature_type(const struct MapLocation* loc, int feature_id);
//bool map_util_is_floor(struct MapLocation* loc);
//bool map_util_is_horizontal_wall(struct MapLocation* loc);
//bool map_util_is_solid_rock(struct MapLocation* loc);
//bool map_util_is_vertical_wall(struct MapLocation* loc);
//bool map_util_is_wall(struct MapLocation* loc);

#endif
