#ifndef YUN_MAP_GEN_MAP_GEN_CELL_H
#define YUN_MAP_GEN_MAP_GEN_CELL_H

#include "map_defs.h"
#include "map_gen/map_gen_location.h"

#include <scieppend/core/array.h>

struct RNG;

struct MapGenCell
{
    int cell_x;
    int cell_y;
    int world_x;
    int world_y;
    struct MapGenLocation locs[g_map_cell_height][g_map_cell_width];
    struct Array rooms;
    struct RNG* rng;
};

void map_gen_cell_init(struct MapGenCell* cell, int x, int y, int seed);
void map_gen_cell_uninit(struct MapGenCell* cell);
struct MapGenLocation* map_gen_cell_get_location(struct MapGenCell* cell, int x, int y);
struct MapGenLocation* map_gen_cell_get_location_relative(struct MapGenCell* cell, int x, int y);
bool map_gen_cell_is_in_bounds(struct MapGenCell* cell, int x, int y);

#endif
