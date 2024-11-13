#include "map_gen_maze.h"

#include "components/feature_component.h"
#include "components/gfx_terminal_component.h"
#include "components/terrain_component.h"

#include "entities.h"
#include "feature.h"
#include "map_cell.h"
#include "map_gen_utils.h"
#include "map_location.h"
#include "map_room.h"
#include "map_utils.h"
#include "terrain.h"
#include "util.h"

#include <scieppend/core/list.h>
#include <scieppend/core/log.h>
#include <scieppend/core/tasking.h>

#include <stddef.h>
#include <string.h>

struct _MapGenMazeState
{
    struct MapCell* cell;
    const struct Terrain* floor;
    const struct Terrain* corridor;
    const struct Feature* rock;
    const struct Feature* wallh;
    const struct Feature* wallv;
};

static void _make_feature(struct MapLocation* loc, const struct Feature* feature)
{
    if(loc->feature == C_NULL_ENTITY_HANDLE)
    {
        loc->feature = entity_create();
        add_position_component(loc->feature, loc->x, loc->y, 1);
        add_gfx_component(loc->feature, feature->symbol);
        add_feature_component(loc->feature, feature->id_hash);
    }
    else
    {
        struct FeatureComponent* fet_c = entity_get_component(loc->feature, g_FeatureComponent_id);
        fet_c->id = feature->id_hash;
        entity_unget_component(loc->feature, g_FeatureComponent_id);

        struct GFXTerminalComponent* gfx_c  = entity_get_component(loc->feature, g_GFXTerminalComponent_id);
        gfx_c->base_symbol = feature->symbol;
        gfx_c->symbol = *feature->symbol;
        entity_unget_component(loc->feature, g_GFXTerminalComponent_id);
    }
}

static void _make_terrain(struct MapLocation* loc, const struct Terrain* terrain)
{
    if(loc->terrain == C_NULL_ENTITY_HANDLE)
    {
        loc->terrain = entity_create();
        add_position_component(loc->terrain, loc->x, loc->y, 0);
        add_gfx_component(loc->terrain, terrain->symbol);
        add_terrain_component(loc->terrain, terrain->id_hash);
    }
    else
    {
        struct TerrainComponent* ter_c = entity_get_component(loc->terrain, g_TerrainComponent_id);
        ter_c->id = terrain->id_hash;
        entity_unget_component(loc->terrain, g_TerrainComponent_id);

        struct GFXTerminalComponent* gfx_c  = entity_get_component(loc->terrain, g_GFXTerminalComponent_id);
        gfx_c->base_symbol = terrain->symbol;
        gfx_c->symbol = *terrain->symbol;
        entity_unget_component(loc->terrain, g_GFXTerminalComponent_id);
    }
}

///* Checks to see if a location is a viable maze node
// * Valid maze nodes need to have only 1 connection to another corridor tile.
// * They can also only have 2 of 4 diagonal squares as corridor tiles so long as those tiles
// * are orthogonal to the single connecting corridor tile.
// */
static bool _is_valid_maze_node(struct _MapGenMazeState* state, struct MapLocation* loc)
{
    struct MapCell* cell = state->cell;

    if(loc->terrain != C_NULL_ENTITY_HANDLE || map_util_is_border_loc(cell, loc))
    {
        return false;
    }

    int conn_count = 0;

    struct MapLocation* tmp = NULL;
    struct MapLocation* conn = NULL;

    tmp = map_cell_get_location(cell, loc->x-1, loc->y);
    if(tmp && map_util_is_terrain_type(tmp, state->corridor->id_hash))
    {
        conn_count++;
        conn = tmp;
    }

    tmp = map_cell_get_location(cell, loc->x+1, loc->y);
    if(tmp && map_util_is_terrain_type(tmp, state->corridor->id_hash))
    {
        conn_count++;
        conn = tmp;
    }

    tmp = map_cell_get_location(cell, loc->x, loc->y-1);
    if(tmp && map_util_is_terrain_type(tmp, state->corridor->id_hash))
    {
        conn_count++;
        conn = tmp;
    }

    tmp = map_cell_get_location(cell, loc->x, loc->y+1);
    if(tmp && map_util_is_terrain_type(tmp, state->corridor->id_hash))
    {
        conn_count++;
        conn = tmp;
    }

    if(conn_count > 1)
    {
        return false;
    }

    int xoff = loc->x - conn->x;
    int yoff = loc->y - conn->y;

    if(xoff == -1)
    {
        if(map_util_is_terrain_type(map_cell_get_location(cell, loc->x-1, loc->y-1), state->corridor->id_hash) ||
           map_util_is_terrain_type(map_cell_get_location(cell, loc->x-1, loc->y+1), state->corridor->id_hash))
        {
            return false;
        }
    }
    else if(xoff == 1)
    {
        if(map_util_is_terrain_type(map_cell_get_location(cell, loc->x+1, loc->y-1), state->corridor->id_hash) ||
           map_util_is_terrain_type(map_cell_get_location(cell, loc->x+1, loc->y+1), state->corridor->id_hash))
        {
            return false;
        }
    }
    else if(yoff == 1)
    {
        if(map_util_is_terrain_type(map_cell_get_location(cell, loc->x-1, loc->y+1), state->corridor->id_hash) ||
           map_util_is_terrain_type(map_cell_get_location(cell, loc->x+1, loc->y+1), state->corridor->id_hash))
        {
            return false;
        }
    }
    else
    {
        if(map_util_is_terrain_type(map_cell_get_location(cell, loc->x-1, loc->y-1), state->corridor->id_hash) ||
           map_util_is_terrain_type(map_cell_get_location(cell, loc->x+1, loc->y-1), state->corridor->id_hash))
        {
            return false;
        }
    }

   return true;
}

///* Check if a location is a valid starting node for a maze
// * Valid maze starting nodes are solid rock surrounded by 8 other solid rock */
bool _is_maze_snode(struct _MapGenMazeState* state, struct MapLocation* loc)
{
    struct MapCell* cell = state->cell;

    if (loc->terrain != C_NULL_ENTITY_HANDLE || map_util_is_border_loc(cell, loc))
    {
        return false;
    }

    for(int x = -1; x < 2; ++x)
    for(int y = -1; y < 2; ++y)
    {
        if(x == 0 && y == 0)
        {
            continue;
        }

        struct MapLocation* adjacent = map_cell_get_location(cell, loc->x + x, loc->y + y);
        if( adjacent == NULL || adjacent->terrain != C_NULL_ENTITY_HANDLE )
        {
            return false;
        }
    }

    return true;
}

/* Finds a maze starting node */
struct MapLocation* _get_maze_snode(struct _MapGenMazeState* state)
{
    struct MapCell* cell = state->cell;

    for(int x = cell->world_x + 1; x < cell->world_x + g_map_cell_width - 1; x++)
    for(int y = cell->world_y + 1; y < cell->world_y + g_map_cell_height - 1; y++)
    {
        struct MapLocation* loc = map_cell_get_location(cell, x, y);
        if(_is_maze_snode(state, loc))
        {
            return loc;
        }
    }

    return NULL;
}

///* Attempts to get a location at random orthogonal to a maze node to continue the maze with.
// * It will fall back to just checking every single orthogonal direction if the randomly picked
// * direction is not valid. */
//struct MapLocation* _get_valid_maze_node(struct MapCell* cell, struct MapLocation* loc)
//{
//    int x = loc->x;
//    int y = loc->y;
//
//    int xoff = 0;
//    int yoff = 0;
//
//    int dir = random_int(0, 3);
//
//    switch(dir)
//    {
//        case 0: xoff = -1; break;
//        case 1: xoff = 1; break;
//        case 2: yoff = -1; break;
//        case 3: yoff= 1; break;
//    }
//
//    struct MapLocation* next;
//    next = map_cell_get_location(cell, x+xoff, y+yoff);
//    if(_is_valid_maze_node(cell, next))
//    {
//        return next;
//    }
//
//    next = map_cell_get_location(cell, x+1, y);
//    if(_is_valid_maze_node(cell, next))
//    {
//        return next;
//    }
//
//    next = map_cell_get_location(cell, x-1, y);
//    if(_is_valid_maze_node(cell, next))
//    {
//        return next;
//    }
//
//    next = map_cell_get_location(cell, x, y+1);
//    if(_is_valid_maze_node(cell, next))
//    {
//        return next;
//    }
//
//    next = map_cell_get_location(cell, x, y-1);
//    if(_is_valid_maze_node(cell, next))
//    {
//        return next;
//    }
//
//    return NULL;
//}

void _flood_fill_maze(struct _MapGenMazeState* state, struct MapLocation* loc)
{
    struct MapCell* cell = state->cell;

    struct List loc_list;
    list_init(&loc_list);

    map_util_enlist_orthogonals_random_order(cell, loc, &loc_list);

    do
    {
        // Picking from head is breadth first, and results in long straight corridors
        // Picking from tail is depth first, and results in twisty corridors
        struct MapLocation* next = loc_list.tail->data;
        list_rm(&loc_list, loc_list.tail);

        if(!_is_valid_maze_node(state, next))
        {
            continue;
        }

        _make_terrain(next, state->corridor);
        map_util_enlist_orthogonals_random_order(cell, next, &loc_list);
    }
    while(loc_list.count > 0);
}

static bool _is_potential_room_entrance(struct _MapGenMazeState* state, struct MapLocation* loc)
{
    struct MapCell* cell = state->cell;

    struct MapLocation* left  = map_cell_get_location(cell, loc->x-1, loc->y);
    struct MapLocation* right = map_cell_get_location(cell, loc->x+1, loc->y);
    struct MapLocation* up    = map_cell_get_location(cell, loc->x, loc->y-1);
    struct MapLocation* down  = map_cell_get_location(cell, loc->x, loc->y+1);

    if((up && map_util_is_terrain_type(up, state->corridor->id_hash)) || (down && map_util_is_terrain_type(down, state->corridor->id_hash)))
    {
        return true;
    }

    if((left && map_util_is_terrain_type(left, state->corridor->id_hash)) || (right && map_util_is_terrain_type(right, state->corridor->id_hash)))
    {
        return true;
    }

    return false;
}

///* Picks wall sections on each wall at random until it finds one that has a corridor next to it.
// * It converts that wall section into an entryway into the room.
// * TODO: Generate a door object in the entryway tile */
void _make_doors(struct _MapGenMazeState* state)
{
    struct MapCell* cell = state->cell;
    struct MapLocation* connectors[36];

    for(int i = 0; i < array_count(&cell->rooms); ++i)
    {
        struct Room* room = array_get(&cell->rooms, i);

        int cidx = 0;
        const int ya = room->y - 1;
        const int yb = room->y + room->h + 1;
        const int xa = room->x - 1;
        const int xb = room->x + room->w + 1;

        // North wall
        if(map_cell_is_in_bounds(cell, 0, ya))
        {
            for(int x = room->x + 1, y = room->y; x < (room->x + room->w - 1); ++x)
            {
                if(_is_potential_room_entrance(state, map_cell_get_location(cell, x, y)))
                {
                    connectors[cidx++] = map_cell_get_location(cell, x, y);
                }
            }
        }

        // South wall
        if(map_cell_is_in_bounds(cell, 0, yb))
        {
            for(int x = room->x + 1, y = room->y + room->h - 1; x < (room->x + room->w - 1); ++x)
            {
                if(_is_potential_room_entrance(state, map_cell_get_location(cell, x, y)))
                {
                    connectors[cidx++] = map_cell_get_location(cell, x, y);
                }
            }
        }


        // West wall
        if(map_cell_is_in_bounds(cell, xa, 0))
        {
            for(int x = room->x, y = room->y; y < (room->y + room->h - 1); y++)
            {
                if(_is_potential_room_entrance(state, map_cell_get_location(cell, x, y)))
                {
                    connectors[cidx++] = map_cell_get_location(cell, x, y);
                }
            }
        }

        // East wall
        if(map_cell_is_in_bounds(cell, xb, 0))
        {
            for(int x = room->x + room->w - 1, y = room->y; y < (room->y + room->h - 1); y++)
            {
                if(_is_potential_room_entrance(state, map_cell_get_location(cell, x, y)))
                {
                    connectors[cidx++] = map_cell_get_location(cell, x, y);
                }
            }
        }

        if(--cidx == -1)
        {
            return;
        }

        int doors_to_make = random_int(1, ((room->w % 2) + (room->h % 2) + room->w + room->h));
        for(int door_count = 0; door_count < doors_to_make;)
        {
            int which = random_int(0, cidx);

            if(_is_potential_room_entrance(state, connectors[which]))
            {
                _make_terrain(connectors[which], state->floor);
                entity_destroy(connectors[which]->feature);
                connectors[which]->feature = C_NULL_ENTITY_HANDLE;
                ++door_count;
            }

            // Switch in the last one in the array
            connectors[which] = connectors[--cidx];

            if(cidx == -1)
            {
                break;
            }
        }
    }
}

static inline bool _is_connector(const struct _MapGenMazeState* state, const struct MapLocation* loc)
{
    return !map_util_is_feature_type(loc, state->rock->id_hash) &&
           (map_util_is_terrain_type(loc, state->corridor->id_hash) || map_util_is_terrain_type(loc, state->floor->id_hash));
}

/* Checks if a given location is a deadend.
 * Deadends have only one connecting corridor tile */
static bool _is_maze_deadend(struct _MapGenMazeState* state, struct MapLocation* loc)
{
    struct MapCell* cell = state->cell;

    if(loc->feature != C_NULL_ENTITY_HANDLE ||  !map_util_is_terrain_type(loc, state->corridor->id_hash))
    {
        return false;
    }

    const struct MapLocation* check = NULL;
    int conn_count = 0;

    for(int off = -1; off < 2; off += 2)
    {
        check = map_cell_get_location(cell, loc->x + off, loc->y);
        if(_is_connector(state, check))
        {
            ++conn_count;
        }

        check = map_cell_get_location(cell, loc->x, loc->y + off);
        if(_is_connector(state, check))
        {
            ++conn_count;
        }
    }

    return conn_count == 1;
}

/* Finds and sets the input arg to the first deadend it finds.
 * Returns true if it finds a deadend. */
static struct MapLocation* _get_maze_deadend(struct _MapGenMazeState* state)
{
    struct MapCell* cell = state->cell;

    for(int x = cell->world_x; x < cell->world_x + g_map_cell_width; x++)
    for(int y = cell->world_y; y < cell->world_y + g_map_cell_height; y++)
    {
        struct MapLocation* loc = map_cell_get_location(cell, x, y);
        if(_is_maze_deadend(state, loc))
        {
            return loc;
        }
    }

    return NULL;
}

/* Finds the next deadend node next to the given one.
 */
struct MapLocation* _get_next_deadend_node(struct _MapGenMazeState* state, struct MapLocation* loc)
{
    struct MapCell* cell = state->cell;
    struct MapLocation* next = NULL;

    for(int offset = -1; offset < 2; offset += 2)
    {
        next = map_cell_get_location(cell, loc->x + offset, loc->y);
        if(next && _is_maze_deadend(state, next))
        {
            return next;
        }

        next = map_cell_get_location(cell, loc->x, loc->y + offset);
        if(next && _is_maze_deadend(state, next))
        {
            return next;
        }
    }

    return NULL;
}

static void _back_fill_deadends(struct _MapGenMazeState* state, struct MapLocation* loc)
{
    struct MapLocation* next = NULL;
    while((next = _get_next_deadend_node(state, loc)))
    {
        _make_feature(next, state->rock);
        loc = next;
    }
}

void map_gen_maze(struct _MapGenMazeState* state)
{
    struct MapLocation* tmp = NULL;

    while((tmp = _get_maze_snode(state)))
    {
        _make_terrain(tmp, state->corridor);
        _flood_fill_maze(state, tmp);
    }

    _make_doors(state);

    while((tmp = _get_maze_deadend(state)))
    {
        _make_feature(tmp, state->rock);
        _back_fill_deadends(state, tmp);
    }
}

static int _gen_maze_task_func(void* state)
{
    struct _MapGenMazeState* maze_state = (struct _MapGenMazeState*)state;
    map_gen_maze(maze_state);
    return TASK_STATUS_SUCCESS;
}

struct Task* map_gen_maze_async(struct MapCell* cell)
{
    struct _MapGenMazeState state;
    state.cell = cell;
    state.wallh = feature_look_up_by_id("walh");
    state.wallv = feature_look_up_by_id("walv");
    state.rock = feature_look_up_by_id("sroc");
    state.floor = terrain_look_up_by_id("flor");
    state.corridor = terrain_look_up_by_id("crdr");

    struct Task* task = task_new("Gen maze", &_gen_maze_task_func, NULL, &state, sizeof(state));
    tasker_add_task(g_tasker, task);

    log_msg(LOG_DEBUG, "Added gen maze task.");

    return task;
}
