#include "map_utils.h"

#include "components/feature_component.h"
#include "components/terrain_component.h"
#include "feature.h"
#include "map_cell.h"
#include "map_location.h"
#include "util.h"

#include <scieppend/core/ecs.h>
#include <scieppend/core/list.h>

#include <string.h>

void map_util_enlist_orthogonals_random_order(struct MapCell* cell, struct MapLocation* loc, struct List* list)
{
    int ortho_dirs[4] = { 1, 2, 3, 4 };
    int dir_count = 3;
    struct MapLocation* next = NULL;

    while(dir_count > -1)
    {
        int random_index = random_int(0, dir_count);
        switch(ortho_dirs[random_index])
        {
            case 1: // Up
            {
                next = map_cell_get_location(cell, loc->x, loc->y-1);
                break;
            }
            case 2: // Down
            {
                next = map_cell_get_location(cell, loc->x, loc->y+1);
                break;
            }
            case 3: // Left
            {
                next = map_cell_get_location(cell, loc->x-1, loc->y);
                break;
            }
            case 4: // Right
            {
                next = map_cell_get_location(cell, loc->x+1, loc->y);
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

bool map_util_is_border_loc(struct MapCell* cell, struct MapLocation* loc)
{
    return (loc->x == cell->world_x || loc->y == cell->world_y || loc->x == cell->world_x + g_map_cell_width - 1 || loc->y == cell->world_y + g_map_cell_height - 1);
}

//bool map_util_is_corner_wall(struct MapLocation* loc)
//{
//    return loc->feature && (strcmp(loc->feature->id, "walc") == 0);
//}

bool map_util_is_terrain_type(const struct MapLocation* loc, int terrain_id)
{
    bool ret = false;

    if(loc->terrain != C_NULL_ENTITY_HANDLE)
    {
        const struct TerrainComponent* terrain_c = entity_get_readonly_component(loc->terrain, g_TerrainComponent_id);
        ret = terrain_c && terrain_c->id == terrain_id;
        entity_unget_readonly_component(loc->terrain, g_TerrainComponent_id);
    }

    return ret;
}

bool map_util_is_feature_type(const struct MapLocation* loc, int feature_id)
{
    bool ret = false;

    if(loc->feature != C_NULL_ENTITY_HANDLE)
    {
        const struct FeatureComponent* feature_c = entity_get_readonly_component(loc->terrain, g_FeatureComponent_id);
        ret = feature_c && feature_c->id == feature_id;
        entity_unget_readonly_component(loc->feature, g_FeatureComponent_id);
    }

    return ret;
}

//bool map_util_is_floor(struct MapLocation* loc)
//{
//    return !loc->feature && loc->symbol.sym == '.';
//}
//
//bool map_util_is_horizontal_wall(struct MapLocation* loc)
//{
//    return loc->feature && (strcmp(loc->feature->id, "walh") == 0);
//}
//
//bool map_util_is_solid_rock(struct MapLocation* loc)
//{
//    return loc->feature && (strcmp(loc->feature->id, "sroc") == 0);
//}
//
//bool map_util_is_vertical_wall(struct MapLocation* loc)
//{
//    return loc->feature && (strcmp(loc->feature->id, "walv") == 0);
//}
//
//bool map_util_is_wall(struct MapLocation* loc)
//{
//    return (map_util_is_vertical_wall(loc)   ||
//            map_util_is_horizontal_wall(loc) ||
//            map_util_is_corner_wall(loc));
//}
