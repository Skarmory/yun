#ifndef YUN_MAP_H
#define YUN_MAP_H

#include "defs.h"
#include "list.h"
#include "mon_attr.h"

#include <stdbool.h>

#define MROWS 40
#define MCOLS 80

struct List;
struct MapCell;
struct MapLocation;
struct Mon;

/**
 * Contains the map data
 */
struct Map
{
    int width;
    int height;
    List cell_list;
};

void display_map(void);

struct Map* map_new(int width, int height);
void map_free(struct Map* map);

struct MapCell* map_get_cell_by_world_coord(struct Map* map, int x, int y);
struct MapCell* map_get_cell_by_map_coord(struct Map* map, int x, int y);
void map_get_screen_coord_by_world_coord(struct Map* map, int world_x, int world_y, int* screen_x, int* screen_y);

struct MapLocation* map_get_location(struct Map* map, int x, int y);
struct MapLocation* map_get_location_offset_by_direction(struct Map* map, struct MapLocation* loc, int input_keycode);

bool map_add_mon(struct Map* map, struct Mon* mon, int x, int y);
bool map_has_mon(struct Map* map, int x, int y);

extern struct Map* g_cmap;

#endif
