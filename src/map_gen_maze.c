#include "map_gen_maze.h"
#include "map_gen_utils.h"

#include "map_cell.h"
#include "map_location.h"
#include "map_room.h"
#include "map_utils.h"

#include "feature.h"
#include "log.h"
#include "tasking.h"
#include "util.h"

#include <stddef.h>
#include <string.h>

static void _make_solid_rock(struct MapLocation* loc)
{
    loc->feature = feature_look_up_by_id("sroc");
    loc->symbol.sym = ' ';
    loc->symbol.fg = g_colours[CLR_DEFAULT];
    loc->symbol.bg = g_colours[CLR_DEFAULT];
}

/* Checks to see if a location is a viable maze node
 * Valid maze nodes need to have only 1 connection to another corridor tile.
 * They can also only have 2 of 4 diagonal squares as corridor tiles so long as those tiles
 * are orthogonal to the single connecting corridor tile.
 */
bool _is_valid_maze_node(struct MapCell* cell, struct MapLocation* loc)
{
    if(!map_util_is_solid_rock(loc) || map_util_is_border_loc(cell, loc))
    {
        return false;
    }

    int conn_count = 0;

    struct MapLocation* tmp = NULL;
    struct MapLocation* conn = NULL;

    tmp = map_cell_get_location(cell, loc->x-1, loc->y);
    if(tmp && map_util_is_corridor(tmp))
    {
        conn_count++;
        conn = tmp;
    }

    tmp = map_cell_get_location(cell, loc->x+1, loc->y);
    if(tmp && map_util_is_corridor(tmp))
    {
        conn_count++;
        conn = tmp;
    }

    tmp = map_cell_get_location(cell, loc->x, loc->y-1);
    if(tmp && map_util_is_corridor(tmp))
    {
        conn_count++;
        conn = tmp;
    }

    tmp = map_cell_get_location(cell, loc->x, loc->y+1);
    if(tmp && map_util_is_corridor(tmp))
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
        if(map_util_is_corridor(map_cell_get_location(cell, loc->x-1, loc->y-1)) || map_util_is_corridor(map_cell_get_location(cell, loc->x-1, loc->y+1)))
        {
            return false;
        }
    }
    else if(xoff == 1)
    {
        if(map_util_is_corridor(map_cell_get_location(cell, loc->x+1, loc->y-1)) || map_util_is_corridor(map_cell_get_location(cell, loc->x+1, loc->y+1)))
        {
            return false;
        }
    }
    else if(yoff == 1)
    {
        if(map_util_is_corridor(map_cell_get_location(cell, loc->x-1, loc->y+1)) || map_util_is_corridor(map_cell_get_location(cell, loc->x+1, loc->y+1)))
        {
            return false;
        }
    }
    else
    {
        if(map_util_is_corridor(map_cell_get_location(cell, loc->x-1, loc->y-1)) || map_util_is_corridor(map_cell_get_location(cell, loc->x+1, loc->y-1)))
        {
            return false;
        }
    }

   return true;
}

/* Check if a location is a valid starting node for a maze
 * Valid maze starting nodes are solid rock surrounded by 8 other solid rock */
bool _is_maze_snode(struct MapCell* cell, struct MapLocation* loc)
{
    if(!map_util_is_solid_rock(loc) || map_util_is_border_loc(cell, loc))
    {
        return false;
    }

    int x = loc->x;
    int y = loc->y;

    if(map_util_is_solid_rock(map_cell_get_location(cell, x-1, y-1)) && map_util_is_solid_rock(map_cell_get_location(cell, x, y-1)) && map_util_is_solid_rock(map_cell_get_location(cell, x+1, y-1)) &&
       map_util_is_solid_rock(map_cell_get_location(cell, x-1, y))   && map_util_is_solid_rock(map_cell_get_location(cell, x+1, y)) &&
       map_util_is_solid_rock(map_cell_get_location(cell, x-1, y+1)) && map_util_is_solid_rock(map_cell_get_location(cell, x, y+1)) && map_util_is_solid_rock(map_cell_get_location(cell, x+1, y+1)))
    {
        return true;
    }

    return false;
}

/* Finds a maze starting node */
struct MapLocation* _get_maze_snode(struct MapCell* cell)
{
    for(int x = cell->world_x; x < cell->world_x + g_map_cell_width; x++)
    for(int y = cell->world_y; y < cell->world_y + g_map_cell_height; y++)
    {
        struct MapLocation* loc = map_cell_get_location(cell, x, y);
        if(_is_maze_snode(cell, loc))
        {
            return loc;
        }
    }

    return NULL;
}

/* Attempts to get a location at random orthogonal to a maze node to continue the maze with.
 * It will fall back to just checking every single orthogonal direction if the randomly picked
 * direction is not valid. */
struct MapLocation* _get_valid_maze_node(struct MapCell* cell, struct MapLocation* loc)
{
    int x = loc->x;
    int y = loc->y;

    int xoff = 0;
    int yoff = 0;

    int dir = random_int(0, 3);

    switch(dir)
    {
        case 0: xoff = -1; break;
        case 1: xoff = 1; break;
        case 2: yoff = -1; break;
        case 3: yoff= 1; break;
    }

    struct MapLocation* next;
    next = map_cell_get_location(cell, x+xoff, y+yoff);
    if(_is_valid_maze_node(cell, next))
    {
        return next;
    }

    next = map_cell_get_location(cell, x+1, y);
    if(_is_valid_maze_node(cell, next))
    {
        return next;
    }

    next = map_cell_get_location(cell, x-1, y);
    if(_is_valid_maze_node(cell, next))
    {
        return next;
    }

    next = map_cell_get_location(cell, x, y+1);
    if(_is_valid_maze_node(cell, next))
    {
        return next;
    }

    next = map_cell_get_location(cell, x, y-1);
    if(_is_valid_maze_node(cell, next))
    {
        return next;
    }

    return NULL;
}

void _flood_fill_maze(struct MapCell* cell, struct MapLocation* loc)
{
    struct List loc_list;
    list_init(&loc_list);

    map_util_enlist_orthogonals_random_order(cell, loc, &loc_list);

    do
    {
        // Picking from head is breadth first, and results in long straight corridors
        // Picking from tail is depth first, and results in twisty corridors
        struct MapLocation* next = loc_list.tail->data;
        list_rm(&loc_list, loc_list.tail);

        if(!_is_valid_maze_node(cell, next))
        {
            continue;
        }

        map_gen_util_make_corridor(next);
        map_util_enlist_orthogonals_random_order(cell, next, &loc_list);
    }
    while(loc_list.count > 0);
}

static bool _is_potential_room_entrance(struct MapCell* cell, struct MapLocation* loc)
{
    struct MapLocation* left  = map_cell_get_location(cell, loc->x-1, loc->y);
    struct MapLocation* right = map_cell_get_location(cell, loc->x+1, loc->y);
    struct MapLocation* up    = map_cell_get_location(cell, loc->x, loc->y-1);
    struct MapLocation* down  = map_cell_get_location(cell, loc->x, loc->y+1);

    if(map_util_is_horizontal_wall(left) && map_util_is_horizontal_wall(right) && (map_util_is_corridor(up) || map_util_is_corridor(down)))
    {
        return true;
    }

    if(map_util_is_vertical_wall(up) && map_util_is_vertical_wall(down) && (map_util_is_corridor(left) || map_util_is_corridor(right)))
    {
        return true;
    }

    return false;
}

/* Picks wall sections on each wall at random until it finds one that has a corridor next to it.
 * It converts that wall section into an entryway into the room.
 * TODO: Generate a door object in the entryway tile */
void _make_doors(struct MapCell* cell)
{
    struct Room* room;

    // TODO: This should equal the size of the perimeter of the maximum sized room, make a constant
    struct MapLocation* connectors[36];

    struct ListNode* rnode;
    list_for_each(&cell->room_list, rnode)
    {
        room = rnode->data;

        int cidx = 0;
        int x, y;

        // North wall
        x = room->x+1;
        y = room->y;
        if(y-1 > cell->world_y)
        {
            for(; x < (room->x + room->w - 1); x++)
            {
                if(map_util_is_corridor(map_cell_get_location(cell, x, y-1)))
                {
                    connectors[cidx] = map_cell_get_location(cell, x, y);
                    cidx++;
                }
            }
        }

        // South wall
        x = room->x+1;
        y = room->y + room->h-1;
        if(y+1 < cell->world_y+g_map_cell_height-1)
        {
            for(; x < (room->x + room->w - 1); x++)
            {
                if(map_util_is_corridor(map_cell_get_location(cell, x, y+1)))
                {
                    connectors[cidx] = map_cell_get_location(cell, x, y);
                    cidx++;
                }
            }
        }

        // West wall
        x = room->x;
        y = room->y+1;
        if(x-1 > cell->world_x)
        {
            for(; y < (room->y + room->h - 1); y++)
            {
                if(map_util_is_corridor(map_cell_get_location(cell, x-1, y)))
                {
                    connectors[cidx] = map_cell_get_location(cell, x, y);
                    cidx++;
                }
            }
        }

        // East wall
        x = room->x + room->w - 1;
        y = room->y+1;
        if(x+1 < cell->world_x+g_map_cell_width-1)
        {
            for(; y < (room->y + room->h - 1); y++)
            {
                if(map_util_is_corridor(map_cell_get_location(cell, x+1, y)))
                {
                    connectors[cidx] = map_cell_get_location(cell, x, y);
                    cidx++;
                }
            }
        }

        int doors_to_make = random_int(1, ((room->w%2) + (room->h%2) + room->w + room->h));
        for(int door_count = 0; door_count < doors_to_make;)
        {
            if(cidx == 0)
            {
                break;
            }

            int which = random_int(0, cidx-1);

            if(_is_potential_room_entrance(cell, connectors[which]))
            {
                map_gen_util_make_floor(connectors[which]);
                ++door_count;
            }

            // Switch in the last one in the array
            connectors[which] = connectors[cidx-1];
            --cidx;
        }
    }
}

/* Checks if a given location is a deadend.
 * Deadends have only one connecting corridor tile */
bool _is_maze_deadend(struct MapCell* cell, struct MapLocation* loc)
{
    if(!map_util_is_corridor(loc) || map_util_is_border_loc(cell, loc))
    {
        return false;
    }

    int x = loc->x;
    int y = loc->y;
    int conn_count = 0;

    if(map_util_is_corridor(map_cell_get_location(cell, x-1, y)) || map_util_is_floor(map_cell_get_location(cell, x-1, y)))
    {
        conn_count++;
    }

    if(map_util_is_corridor(map_cell_get_location(cell, x+1, y)) || map_util_is_floor(map_cell_get_location(cell, x+1, y)))
    {
        conn_count++;
    }

    if(map_util_is_corridor(map_cell_get_location(cell, x, y+1)) || map_util_is_floor(map_cell_get_location(cell, x, y+1)))
    {
        conn_count++;
    }

    if(map_util_is_corridor(map_cell_get_location(cell, x, y-1)) || map_util_is_floor(map_cell_get_location(cell, x, y-1)))
    {
        conn_count++;
    }

    return conn_count <= 1;
}

/* Finds and sets the input arg to the first deadend it finds.
 * Returns true if it finds a deadend. */
struct MapLocation* _get_maze_deadend(struct MapCell* cell)
{
    for(int x = cell->world_x; x < cell->world_x + g_map_cell_width; x++)
    for(int y = cell->world_y; y < cell->world_y + g_map_cell_height; y++)
    {
        struct MapLocation* loc = map_cell_get_location(cell, x, y);
        if(_is_maze_deadend(cell, loc))
        {
            return loc;
        }
    }

    return NULL;
}

/*
 * Finds the next deadend node next to the given one.
 */
struct MapLocation* _get_next_deadend_node(struct MapCell* cell, struct MapLocation* loc)
{
    struct MapLocation* next;
    int x = loc->x;
    int y = loc->y;

    next = map_cell_get_location(cell, x-1, y);
    if(next && _is_maze_deadend(cell, next))
    {
        return next;
    }

    next = map_cell_get_location(cell, x+1, y);
    if(next && _is_maze_deadend(cell, next))
    {
        return next;
    }

    next = map_cell_get_location(cell, x, y-1);
    if(next && _is_maze_deadend(cell, next))
    {
        return next;
    }

    next = map_cell_get_location(cell, x, y+1);
    if(next && _is_maze_deadend(cell, next))
    {
        return next;
    }

    return NULL;
}

void _back_fill_deadends(struct MapCell* cell, struct MapLocation* loc)
{
    struct MapLocation* next;
    while((next = _get_next_deadend_node(cell, loc)))
    {
        _make_solid_rock(next);
        loc = next;
    }
}


void map_gen_maze(struct MapCell* cell)
{
    struct MapLocation* tmp;

    while((tmp = _get_maze_snode(cell)))
    {
        map_gen_util_make_corridor(tmp);
        _flood_fill_maze(cell, tmp);
    }

    _make_doors(cell);

    while((tmp = _get_maze_deadend(cell)))
    {
        _make_solid_rock(tmp);
        _back_fill_deadends(cell, tmp);
    }
}

int gen_maze_task_func(void* state)
{
    struct MapCell* cell = *(struct MapCell**)state;
    map_gen_maze(cell);
    return TASK_STATUS_SUCCESS;
}

void map_gen_maze_async(struct MapCell* cell)
{
    struct Task* task = task_new("Gen maze", gen_maze_task_func, NULL, &cell, sizeof(cell));
    tasker_add_task(g_tasker, task);

    log_msg(LOG_DEBUG, "Added gen maze task.");
}
