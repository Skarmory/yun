#include "map_gen/map_gen_maze.h"

#include "feature.h"
#include "game_world.h"
#include "map_gen/map_gen_cell.h"
#include "map_gen/map_gen_location.h"
#include "map_gen/map_gen_utils.h"
#include "map_room.h"
#include "terrain.h"
#include "util.h"

#include <scieppend/core/ecs_world.h>
#include <scieppend/core/list.h>
#include <scieppend/core/log.h>
#include <scieppend/core/rng.h>
#include <scieppend/core/tasking.h>
#include <stddef.h>
#include <string.h>

struct _MapGenMazeState
{
    struct MapGenCell* cell;
    const struct Terrain* floor;
    const struct Terrain* corridor;
    const struct Feature* rock;
    const struct Feature* wallh;
    const struct Feature* wallv;
};

/**
 * Checks to see if a location is a viable maze node
 * Valid maze nodes need to have only 1 connection to another corridor tile.
 * They can also only have 2 of 4 diagonal squares as corridor tiles so long as those tiles
 * are orthogonal to the single connecting corridor tile.
 */
static bool _is_valid_maze_node(struct _MapGenMazeState* state, struct MapGenLocation* loc)
{
    struct MapGenCell* cell = state->cell;

    if(!mapgen_util_is_solid_rock(loc) || mapgen_util_is_border_loc(cell, loc))
    {
        return false;
    }

    int conn_count = 0;
    struct MapGenLocation* conn = NULL;
    struct MapGenLocation* tmp = NULL;

    tmp = map_gen_cell_get_location(cell, loc->x - 1, loc->y);
    if(tmp && mapgen_util_is_corridor(tmp))
    {
        ++conn_count;
        conn = tmp;
    }

    tmp = map_gen_cell_get_location(cell, loc->x + 1, loc->y);
    if(tmp && mapgen_util_is_corridor(tmp))
    {
        ++conn_count;
        conn = tmp;
    }

    tmp = map_gen_cell_get_location(cell, loc->x, loc->y - 1);
    if(tmp && mapgen_util_is_corridor(tmp))
    {
        ++conn_count;
        conn = tmp;
    }

    tmp = map_gen_cell_get_location(cell, loc->x, loc->y + 1);
    if(tmp && mapgen_util_is_corridor(tmp))
    {
        ++conn_count;
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
        if(mapgen_util_is_corridor(map_gen_cell_get_location(cell, loc->x - 1, loc->y - 1)) ||
           mapgen_util_is_corridor(map_gen_cell_get_location(cell, loc->x - 1, loc->y + 1)))
        {
            return false;
        }
    }
    else if(xoff == 1)
    {
        if(mapgen_util_is_corridor(map_gen_cell_get_location(cell, loc->x + 1, loc->y - 1)) ||
           mapgen_util_is_corridor(map_gen_cell_get_location(cell, loc->x + 1, loc->y + 1)))
        {
            return false;
        }
    }
    else if(yoff == 1)
    {
        if(mapgen_util_is_corridor(map_gen_cell_get_location(cell, loc->x - 1, loc->y + 1)) ||
           mapgen_util_is_corridor(map_gen_cell_get_location(cell, loc->x + 1, loc->y + 1)))
        {
            return false;
        }
    }
    else
    {
        if(mapgen_util_is_corridor(map_gen_cell_get_location(cell, loc->x - 1, loc->y - 1)) ||
           mapgen_util_is_corridor(map_gen_cell_get_location(cell, loc->x + 1, loc->y - 1)))
        {
            return false;
        }
    }

   return true;
}

/**
 * Check if a location is a valid starting node for a maze
 * Valid maze starting nodes are solid rock surrounded by 8 other solid rock
 */
bool _is_maze_snode(struct _MapGenMazeState* state, struct MapGenLocation* loc)
{
    struct MapGenCell* cell = state->cell;

    if (!mapgen_util_is_solid_rock(loc) || mapgen_util_is_border_loc(cell, loc))
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

        struct MapGenLocation* adjacent = map_gen_cell_get_location(cell, loc->x + x, loc->y + y);
        if(adjacent == NULL || !mapgen_util_is_solid_rock(adjacent))
        {
            return false;
        }
    }

    return true;
}

/**
 * Finds a maze starting node
 */
struct MapGenLocation* _get_maze_snode(struct _MapGenMazeState* state)
{
    struct MapGenCell* cell = state->cell;

    for(int x = cell->world_x + 1; x < cell->world_x + g_map_cell_width - 1; x++)
    for(int y = cell->world_y + 1; y < cell->world_y + g_map_cell_height - 1; y++)
    {
        struct MapGenLocation* loc = map_gen_cell_get_location(cell, x, y);
        if(_is_maze_snode(state, loc))
        {
            return loc;
        }
    }

    return NULL;
}

/**
 * Given a valid start node for a maze segment, randomly grab orthogonal locations
 * to continue carving out the maze with.
 */
void _flood_fill_maze(struct _MapGenMazeState* state, struct MapGenLocation* loc)
{
    struct MapGenCell* cell = state->cell;

    struct List loc_list;
    list_init(&loc_list);

    loc->terrain = state->corridor;
    loc->feature = NULL;

    mapgen_util_enlist_orthogonals_random_order(cell, loc, &loc_list);

    do
    {
        // Picking from head is breadth first, and results in long straight corridors
        // Picking from tail is depth first, and results in twisty corridors
        struct MapGenLocation* next = loc_list.tail->data;
        list_rm(&loc_list, loc_list.tail);

        if(!_is_valid_maze_node(state, next))
        {
            continue;
        }

        next->terrain = state->corridor;
        next->feature = NULL;
        mapgen_util_enlist_orthogonals_random_order(cell, next, &loc_list);
    }
    while(loc_list.count > 0);
}

/**
 * A potential room entrance is a wall that has an orthogonally opposed pair of corridor and floor
 * and also two orthogonally opposed walls.
 *
 * e.g. where ? is the location in question:
 *       |      #
 *      #?.    -?-
 *       |      .
 *
 * We don't need to check it is a wall or for the adjacent floor, we already know that to be true.
 */
static bool _is_potential_room_entrance(struct _MapGenMazeState* state, struct MapGenLocation* loc)
{
    struct MapGenCell* cell = state->cell;

    struct MapGenLocation* left  = map_gen_cell_get_location(cell, loc->x-1, loc->y);
    struct MapGenLocation* right = map_gen_cell_get_location(cell, loc->x+1, loc->y);
    struct MapGenLocation* up    = map_gen_cell_get_location(cell, loc->x, loc->y-1);
    struct MapGenLocation* down  = map_gen_cell_get_location(cell, loc->x, loc->y+1);

    if(left == NULL || right == NULL || up == NULL || down == NULL)
    {
        return false;
    }

    // Check horizontal
    if((mapgen_util_is_corridor(up) || mapgen_util_is_corridor(down)) &&
        mapgen_util_is_wall(left)   && mapgen_util_is_wall(right))
    {
        return true;
    }

    // Check vertical
    if((mapgen_util_is_corridor(left) || mapgen_util_is_corridor(right)) &&
        mapgen_util_is_wall(up)   && mapgen_util_is_wall(down))
    {
        return true;
    }

    return false;
}

/**
 * Picks wall sections on each wall at random until it finds one that has a corridor next to it.
 * It converts that wall section into an entryway into the room.
 * It will not generate doors against the edges of the cell.
 * TODO: Generate a door object in the entryway tile
 */
void _make_doors(struct _MapGenMazeState* state)
{
    struct MapGenCell* cell = state->cell;
    struct MapGenLocation* connectors[36];

    for(int i = 0; i < array_count(&cell->rooms); ++i)
    {
        struct Room* room = array_get(&cell->rooms, i);

        int cidx = 0;
        const int ya = room->y - 1;
        const int yb = room->y + room->h + 1;
        const int xa = room->x - 1;
        const int xb = room->x + room->w + 1;

        // North wall
        if(map_gen_cell_is_in_bounds(cell, 0, ya))
        {
            for(int x = room->x + 1, y = room->y; x < (room->x + room->w - 1); ++x)
            {
                if(_is_potential_room_entrance(state, map_gen_cell_get_location(cell, x, y)))
                {
                    connectors[cidx++] = map_gen_cell_get_location(cell, x, y);
                }
            }
        }

        // South wall
        if(map_gen_cell_is_in_bounds(cell, 0, yb))
        {
            for(int x = room->x + 1, y = room->y + room->h - 1; x < (room->x + room->w - 1); ++x)
            {
                if(_is_potential_room_entrance(state, map_gen_cell_get_location(cell, x, y)))
                {
                    connectors[cidx++] = map_gen_cell_get_location(cell, x, y);
                }
            }
        }


        // West wall
        if(map_gen_cell_is_in_bounds(cell, xa, 0))
        {
            for(int x = room->x, y = room->y; y < (room->y + room->h - 1); y++)
            {
                if(_is_potential_room_entrance(state, map_gen_cell_get_location(cell, x, y)))
                {
                    connectors[cidx++] = map_gen_cell_get_location(cell, x, y);
                }
            }
        }

        // East wall
        if(map_gen_cell_is_in_bounds(cell, xb, 0))
        {
            for(int x = room->x + room->w - 1, y = room->y; y < (room->y + room->h - 1); y++)
            {
                if(_is_potential_room_entrance(state, map_gen_cell_get_location(cell, x, y)))
                {
                    connectors[cidx++] = map_gen_cell_get_location(cell, x, y);
                }
            }
        }

        if(--cidx == -1)
        {
            return;
        }

        int doors_to_make = rng_range(cell->rng, 1, ((room->w % 2) + (room->h % 2) + room->w + room->h));
        for(int door_count = 0; door_count < doors_to_make;)
        {
            int which = rng_range(cell->rng, 0, cidx);

            // Check again because making a location a door affected the candidacy of the surrounding
            if(_is_potential_room_entrance(state, connectors[which]))
            {
                connectors[which]->terrain = state->floor;
                connectors[which]->feature = NULL;
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

static inline bool _is_connector(const struct MapGenLocation* loc)
{
    return loc->feature == NULL;
}

/**
 * Checks if a given location is a deadend.
 * Deadends have only one connecting corridor tile, no features, and must be corridor themself.
 */
static bool _is_maze_deadend(struct _MapGenMazeState* state, struct MapGenLocation* loc)
{
    struct MapGenCell* cell = state->cell;

    if(!mapgen_util_is_corridor(loc))
    {
        return false;
    }

    const struct MapGenLocation* check = NULL;
    int conn_count = 0;

    for(int off = -1; off < 2; off += 2)
    {
        check = map_gen_cell_get_location(cell, loc->x + off, loc->y);
        if(_is_connector(check))
        {
            ++conn_count;
        }

        check = map_gen_cell_get_location(cell, loc->x, loc->y + off);
        if(_is_connector(check))
        {
            ++conn_count;
        }
    }

    return conn_count == 1;
}

/**
 * Finds and sets the input arg to the first deadend it finds.
 * Returns true if it finds a deadend.
 */
static struct MapGenLocation* _get_maze_deadend(struct _MapGenMazeState* state)
{
    struct MapGenCell* cell = state->cell;

    for(int x = 0; x < g_map_cell_width; ++x)
    for(int y = 0; y < g_map_cell_height; ++y)
    {
        struct MapGenLocation* loc = map_gen_cell_get_location_relative(cell, x, y);
        if(_is_maze_deadend(state, loc))
        {
            return loc;
        }
    }

    return NULL;
}

/**
 * Finds the next deadend node next to the given one.
 */
struct MapGenLocation* _get_next_deadend_node(struct _MapGenMazeState* state, struct MapGenLocation* loc)
{
    struct MapGenCell* cell = state->cell;
    struct MapGenLocation* next = NULL;

    for(int offset = -1; offset < 2; offset += 2)
    {
        next = map_gen_cell_get_location(cell, loc->x + offset, loc->y);
        if(next != NULL && _is_maze_deadend(state, next))
        {
            return next;
        }

        next = map_gen_cell_get_location(cell, loc->x, loc->y + offset);
        if(next != NULL && _is_maze_deadend(state, next))
        {
            return next;
        }
    }

    return NULL;
}

/**
 * Given a valid maze dead end node, walk back through the deadend and fill it in.
 * This means that all the deadends will be gone and the remaining corridors will
 * be fully connected.
 */
static void _back_fill_deadends(struct _MapGenMazeState* state, struct MapGenLocation* loc)
{
    do
    {
        loc->feature = state->rock;
    }
    while((loc = _get_next_deadend_node(state, loc)));
}

void map_gen_maze(struct _MapGenMazeState* state)
{
    struct MapGenLocation* tmp = NULL;

    while((tmp = _get_maze_snode(state)))
    {
        _flood_fill_maze(state, tmp);
    }

    _make_doors(state);

    //while((tmp = _get_maze_deadend(state)))
    //{
    //    _back_fill_deadends(state, tmp);
    //}
}

static int _gen_maze_task_func(void* state)
{
    struct _MapGenMazeState* maze_state = (struct _MapGenMazeState*)state;
    map_gen_maze(maze_state);
    return TASK_STATUS_SUCCESS;
}

struct Task* map_gen_maze_async(struct MapGenCell* cell)
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
