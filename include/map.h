#ifndef NAXX_MAP_H
#define NAXX_MAP_H

#include "defs.h"
#include "list.h"
#include "mon_attr.h"

#include <stdbool.h>

struct List;
struct MapCell;

#define MROWS 40u
#define MCOLS 80u

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

extern struct Map* cmap;

#endif
