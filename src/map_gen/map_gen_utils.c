#include "map_gen/map_gen_utils.h"

#include "feature.h"
#include "game_world.h"
#include "map_gen/map_gen_cell.h"
#include "map_gen/map_gen_location.h"
#include "terrain.h"
#include "util.h"

#include <scieppend/core/list.h>
#include <scieppend/core/rng.h>
#include <string.h>

void mapgen_util_enlist_orthogonals_random_order(struct MapGenCell* cell, struct MapGenLocation* loc, struct List* list)
{
    int ortho_dirs[4] = { 1, 2, 3, 4 };
    int dir_count = 3;
    struct MapGenLocation* next = NULL;

    while(dir_count > -1)
    {
        int random_index = rng_range(cell->rng, 0, dir_count);
        switch(ortho_dirs[random_index])
        {
            case 1: // Up
            {
                next = map_gen_cell_get_location(cell, loc->x, loc->y-1);
                break;
            }
            case 2: // Down
            {
                next = map_gen_cell_get_location(cell, loc->x, loc->y+1);
                break;
            }
            case 3: // Left
            {
                next = map_gen_cell_get_location(cell, loc->x-1, loc->y);
                break;
            }
            case 4: // Right
            {
                next = map_gen_cell_get_location(cell, loc->x+1, loc->y);
                break;
            }
        }

        // Switch chosen one to the end so it won't be picked again
        int tmp = ortho_dirs[random_index];
        ortho_dirs[random_index] = ortho_dirs[dir_count];
        ortho_dirs[dir_count] = tmp;
        --dir_count;

        if(next)
        {
            list_add(list, next);
        }
    }
}

bool mapgen_util_is_border_loc(struct MapGenCell* cell, struct MapGenLocation* loc)
{
    return (loc->x == cell->world_x || loc->y == cell->world_y || loc->x == cell->world_x + g_map_cell_width - 1 || loc->y == cell->world_y + g_map_cell_height - 1);
}

bool mapgen_util_is_corner_wall(struct MapGenLocation* loc)
{
    return loc->feature && (strcmp(loc->feature->id, "walc") == 0);
}

bool mapgen_util_is_corridor(struct MapGenLocation* loc)
{
    return loc->feature == NULL && (strcmp(loc->terrain->id, "crdr") == 0);
}

bool mapgen_util_is_horizontal_wall(struct MapGenLocation* loc)
{
    return loc->feature && (strcmp(loc->feature->id, "walh") == 0);
}

bool mapgen_util_is_solid_rock(struct MapGenLocation* loc)
{
    return loc->feature && (strcmp(loc->feature->id, "sroc") == 0);
}

bool mapgen_util_is_vertical_wall(struct MapGenLocation* loc)
{
    return loc->feature && (strcmp(loc->feature->id, "walv") == 0);
}

bool mapgen_util_is_wall(struct MapGenLocation* loc)
{
    return (mapgen_util_is_vertical_wall(loc)   ||
            mapgen_util_is_horizontal_wall(loc) ||
            mapgen_util_is_corner_wall(loc));
}
