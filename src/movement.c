#include "movement.h"

#include "map.h"
#include "map_cell.h"
#include "map_location.h"
#include "monster.h"
#include "pathing.h"

#include <stddef.h>

static inline bool _move_is_pathable(struct MapCell* cell, int destx, int desty, MonAttrMoveFlags move_flags)
{
    PathingFlags path_flags = loc_get_pathing(map_cell_get_location(cell, destx, desty));
    return (
        ((path_flags & PATHING_GROUND) && (move_flags & MONATTR_WALKS)) ||
        ((path_flags & PATHING_WATER)  && (move_flags & MONATTR_SWIMS)) ||
        ((path_flags & PATHING_FLYING) && (move_flags & MONATTR_FLIES))
    );
}

bool move_is_pathable(int destx, int desty, MonAttrMoveFlags move_flags)
{
    struct MapCell* cell = map_get_cell_by_world_coord(cmap, destx, desty); 
    if(!cell)
    {
        return false;
    }
    
    return _move_is_pathable(cell, destx, desty, move_flags);
}

static inline bool _move_is_valid(struct MapCell* cell, int destx, int desty, MonAttrMoveFlags move_flags)
{
    return _move_is_pathable(cell, destx, desty, move_flags) && !map_cell_has_mon(cell, destx, desty);
}

bool move_is_valid(int destx, int desty, MonAttrMoveFlags move_flags)
{
    struct MapCell* cell = map_get_cell_by_world_coord(cmap, destx, desty);
    if(!cell)
    {
        return false;
    }

    return _move_is_valid(cell, destx, desty, move_flags);
}

bool move_mon(struct Mon* mon, int newx, int newy)
{
    struct MapCell* old_cell = map_get_cell_by_world_coord(cmap, mon->x, mon->y);
    struct MapCell* new_cell = map_get_cell_by_world_coord(cmap, newx, newy);

    if(!_move_is_valid(new_cell, newx, newy, mon->move_flags))
    {
        return false;
    }

    struct MapLocation* old_loc = map_cell_get_location(old_cell, mon->x, mon->y);
    struct MapLocation* new_loc = map_cell_get_location(new_cell, newx, newy);

    old_loc->mon = NULL;
    new_loc->mon = mon;
    mon->x = newx;
    mon->y = newy;

    if(old_cell != new_cell)
    {
        list_splice_node(&old_cell->mon_list, &new_cell->mon_list, list_find(&old_cell->mon_list, mon));
    }

    return true;
}

