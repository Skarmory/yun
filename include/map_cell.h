#ifndef NAXX_MAP_CELL_H
#define NAXX_MAP_CELL_H

#include "map_location.h"
#include "mon_attr.h"

#include <stdbool.h>

#define g_map_cell_width  100
#define g_map_cell_height 100

struct MapCell
{
    int cell_x;
    int cell_y;
    int world_x;
    int world_y;
    struct MapLocation locs[g_map_cell_width * g_map_cell_height];
    List room_list;
    List mon_list;
};

struct MapCell* map_cell_new(int cell_x, int cell_y);
void map_cell_free(struct MapCell* cell);

struct MapLocation* map_cell_get_location(struct MapCell* cell, int x, int y);
List* map_cell_get_objects(struct MapCell* cell, int x, int y);
void map_cell_add_mon(struct MapCell* cell, struct Mon* mon);
bool map_cell_rm_mon(struct MapCell* cell, struct Mon* mon);
bool map_cell_move_mon(struct MapCell* cell, struct Mon* mon, int newx, int newy);

bool map_cell_has_mon(struct MapCell* cell, int x, int y);
bool map_cell_is_in_bounds(struct MapCell* cell, int x, int y);
bool map_cell_is_pathable(struct MapCell* cell, int x, int y, MonAttrMoveFlags move_flags);
bool map_cell_is_valid_move(struct MapCell* cell, int x, int y, MonAttrMoveFlags move_flags);

#endif
