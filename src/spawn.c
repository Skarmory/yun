#include "spawn.h"

#include "console_command.h"
#include "map.h"
#include "map_cell.h"
#include "monster.h"
#include "mon_type.h"
#include "variant.h"

#include <string.h>

static const char* c_spawn_type_mon = "mon";

enum SpawnType spawn_type_from_string(const char* str)
{
    if(strcmp(str, c_spawn_type_mon) == 0)
    {
        return SPAWN_TYPE_MON;
    }

    return SPAWN_TYPE_UNKNOWN;
}

bool spawn_mon(const char* id, int wx, int wy)
{
    // Check for valid position
    //struct MapCell* cell = map_get_cell_by_world_coord(g_cmap, wx, wy);
    //if(map_cell_has_mon(cell, wx, wy))
    if(map_has_mon(g_cmap, wx, wy))
    {
        return false;
    }

    struct MonType* type = mon_type_look_up_by_id(id);
    struct Mon* mon = mon_new(type, wx, wy);
    //map_cell_add_mon(cell, mon);
    map_add_mon(g_cmap, mon, wx, wy);

    return true;
}

void _spawn_console_command_execute(const struct CommandParams* params)
{
    enum SpawnType type = command_params_get_param(params, 0)->data.as_int;
    const char* id      = command_params_get_param(params, 1)->data.as_str;
    int x               = command_params_get_param(params, 2)->data.as_int;
    int y               = command_params_get_param(params, 3)->data.as_int;

    switch(type)
    {
        case SPAWN_TYPE_MON:
        {
            spawn_mon(id, x, y);
            break;
        }
        case SPAWN_TYPE_UNKNOWN:
        {
            break;
        }
    }
}

void init_spawn_console_command(struct ConsoleCommand* out_command)
{
    console_command_init(out_command, "spawn", _spawn_console_command_execute);
    console_command_add_param_int(out_command, "spawn_type", spawn_type_from_string);
    console_command_add_param_string(out_command, "id", NULL);
    console_command_add_param_int(out_command, "wx", convert_to_int);
    console_command_add_param_int(out_command, "wy", convert_to_int);
}
