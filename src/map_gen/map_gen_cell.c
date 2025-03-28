#include "map_gen/map_gen_cell.h"

#include "map_room.h"

#include <scieppend/core/rng.h>

#include <stddef.h>

void map_gen_cell_init(struct MapGenCell* cell, int x, int y, int seed)
{
    cell->cell_x = x;
    cell->cell_y = y;
    cell->world_x = x * g_map_cell_width;
    cell->world_y = y * g_map_cell_height;
    cell->rng = rng_new(seed);
    array_init(&cell->rooms, sizeof(struct Room), 10, NULL, NULL);

    for(int _y = 0; _y < g_map_cell_height; ++_y)
    for(int _x = 0; _x < g_map_cell_width; ++_x)
    {
        struct MapGenLocation* loc = map_gen_cell_get_location_relative(cell, _x, _y);
        loc->x = cell->world_x + _x;
        loc->y = cell->world_y + _y;
        loc->terrain = NULL;
        loc->feature = NULL;
    }
}

void map_gen_cell_uninit(struct MapGenCell* cell)
{
    array_uninit(&cell->rooms);
    rng_free(cell->rng);
}

struct MapGenLocation* map_gen_cell_get_location(struct MapGenCell* cell, int x, int y)
{
    if(map_gen_cell_is_in_bounds(cell, x, y))
    {
        return &cell->locs[y % g_map_cell_height][x % g_map_cell_width];
    }

    return NULL;
}

struct MapGenLocation* map_gen_cell_get_location_relative(struct MapGenCell* cell, int x, int y)
{
    if(x >= 0 && x < g_map_cell_width && y >= 0 && y < g_map_cell_height)
    {
        return &cell->locs[y % g_map_cell_height][x % g_map_cell_width];
    }

    return NULL;
}

bool map_gen_cell_is_in_bounds(struct MapGenCell* cell, int x, int y)
{
    return (x >= cell->world_x && x < cell->world_x + g_map_cell_width) &&
           (y >= cell->world_y && y < cell->world_y + g_map_cell_height);
}
