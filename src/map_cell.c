#include "map_cell.h"

#include "colour.h"
#include "monster.h"
#include "pathing.h"

#include <stdlib.h>

struct MapCell* map_cell_new(int cell_x, int cell_y)
{
    struct MapCell* cell = (struct MapCell*) malloc(sizeof(struct MapCell));
    cell->cell_x = cell_x;
    cell->cell_y = cell_y;
    cell->world_x = cell_x * g_map_cell_width;
    cell->world_y = cell_y * g_map_cell_height;
    list_init(&cell->room_list);
    list_init(&cell->mon_list);

    for(int i = 0; i < g_map_cell_width; ++i)
    for(int j = 0; j < g_map_cell_height; ++j)
    {
        struct MapLocation* loc = map_cell_get_location(cell, i, j);
        loc->x = cell->world_x + i;
        loc->y = cell->world_y + j;
        loc->symbol.sym  = ' ';
        loc->symbol.fg   = *COL(CLR_WHITE);
        loc->symbol.bg   = *COL(CLR_DEFAULT);
        loc->symbol.attr = 0;
        loc->path_node = new_path_node(loc);
        loc->mon = NULL;
        loc->pathing_flags = 0;
        list_init(&loc->obj_list);
    }

    return cell;
}

void map_cell_free(struct MapCell* cell)
{
    ListNode *node, *n;

    // Free mons
    list_for_each_safe(&cell->mon_list, node, n)
    {
        mon_free(node->data);
        free(node);
    }

    // Free rooms
    list_for_each_safe(&cell->room_list, node, n)
    {
        free(node->data);
        free(node);
    }

    for(int idx = 0; idx < (g_map_cell_width * g_map_cell_height); ++idx)
    {
        free(cell->locs[idx].path_node);
    }

    free(cell);
}

struct MapLocation* map_cell_get_location(struct MapCell* cell, int x, int y)
{
    if(map_cell_is_in_bounds(cell, x, y))
    {
        int _x = (x % g_map_cell_width);
        int _y = (y % g_map_cell_height);
        return &cell->locs[_y * g_map_cell_width + _x];
    }
    return NULL;
}

List* map_cell_get_objects(struct MapCell* cell, int x, int y)
{
    return &map_cell_get_location(cell, x, y)->obj_list;
}

void map_cell_add_mon(struct MapCell* cell, struct Mon* mon)
{
    map_cell_get_location(cell, mon->x, mon->y)->mon = mon;
    list_add(&cell->mon_list, mon);
}

bool map_cell_rm_mon(struct MapCell* cell, struct Mon* mon)
{
    ListNode* node = list_find(&cell->mon_list, mon);
    if(node)
    {
        map_cell_get_location(cell, mon->x, mon->y)->mon = NULL;
        list_rm(&cell->mon_list, node);
        return true;
    }

    return false;
}

bool map_cell_move_mon(struct MapCell* cell, struct Mon* mon, int newx, int newy)
{
    if(!map_cell_is_valid_move(cell, newx, newy, mon->move_flags))
        return false;

    map_cell_get_location(cell, mon->x, mon->y)->mon = NULL;
    map_cell_get_location(cell, newx, newy)->mon = mon;
    mon->x = newx;
    mon->y = newy;

    return true;
}

bool map_cell_has_mon(struct MapCell* cell, int x, int y)
{
    return cell->locs[y * g_map_cell_width + x].mon != NULL;
}

bool map_cell_is_in_bounds(struct MapCell* cell, int x, int y)
{
    return (x >= cell->world_x && x < cell->world_x + g_map_cell_width) &&
           (y >= cell->world_y && y < cell->world_y + g_map_cell_height);
}

bool map_cell_is_pathable(struct MapCell* cell, int x, int y, MonAttrMoveFlags move_flags)
{
    PathingFlags path_flags = map_cell_get_location(cell, x, y)->pathing_flags;
    return (
        ((path_flags & PATHING_GROUND) && (move_flags & MONATTR_WALKS)) ||
        ((path_flags & PATHING_WATER)  && (move_flags & MONATTR_SWIMS)) ||
        ((path_flags & PATHING_FLYING) && (move_flags & MONATTR_FLIES))
    );
}

bool map_cell_is_valid_move(struct MapCell* cell, int x, int y, MonAttrMoveFlags move_flags)
{
    return map_cell_is_in_bounds(cell, x, y) &&
           map_cell_is_pathable(cell, x, y, move_flags) &&
           !map_cell_has_mon(cell, x, y);
}
