#include "spawn.h"

#include "map.h"
#include "map_cell.h"
#include "monster.h"
#include "mon_type.h"

bool spawn_mon(const char* id, int wx, int wy)
{
    // Check for valid position
    struct MapCell* cell = map_get_cell_by_world_coord(cmap, wx, wy);
    if(map_cell_has_mon(cell, wx, wy))
    {
        return false;
    }

    struct MonType* type = mon_type_look_up_by_id(id);
    struct Mon* mon = mon_new(type, wx, wy);
    map_cell_add_mon(cell, mon);

    return true;
}
