#include "map_gen.h"

#include "colour.h"
#include "feature.h"
#include "globals.h"
#include "log.h"
#include "map.h"
#include "map_cell.h"
#include "map_location.h"
#include "map_room.h"
#include "symbol.h"
#include "tasking.h"
#include "util.h"

#include <stdlib.h>
#include <string.h>

// TODO: Check over the maze gen algorithm for performance improvements
// TODO: Ensure that rooms are always connected

enum MapCellGenStage
{
    GEN_STAGE_ROOMS,
    GEN_STAGE_FLOOD_FILL_MAZE,
    GEN_STAGE_DOORS,
    GEN_STAGE_BACK_FILL_MAZE,
    GEN_STAGE_FINISHED
};

struct MapCellGenState
{
    struct MapCell* cell;
    int room_attempts;
    int room_attempts_max;
    List tmp_list;
    enum MapCellGenStage stage;
};

static inline bool _is_border_loc(struct MapCell* cell, struct MapLocation* loc)
{
    return (loc->x == cell->world_x || loc->y == cell->world_y || loc->x == cell->world_x + g_map_cell_width - 1 || loc->y == cell->world_y + g_map_cell_height - 1);
}

static bool _is_vertical_wall(struct MapLocation* loc)
{
    return loc->feature && (strcmp(loc->feature->id, "walv") == 0);
}

static bool _is_horizontal_wall(struct MapLocation* loc)
{
    return loc->feature && (strcmp(loc->feature->id, "walh") == 0);
}

static bool _is_corner_wall(struct MapLocation* loc)
{
    return loc->feature && (strcmp(loc->feature->id, "walc") == 0);
}

static bool _is_solid_rock(struct MapLocation* loc)
{
    return loc->feature && (strcmp(loc->feature->id, "sroc") == 0);
}

static void _make_solid_rock(struct MapLocation* loc)
{
    loc->feature = feature_look_up_by_id("sroc");
    loc->symbol.sym = ' ';
    loc->symbol.fg = g_colours[CLR_DEFAULT];
    loc->symbol.bg = g_colours[CLR_DEFAULT];
}

static inline bool _is_wall(struct MapLocation* loc)
{
    return (_is_vertical_wall(loc)   ||
            _is_horizontal_wall(loc) ||
            _is_corner_wall(loc));
}

static bool _is_corridor(struct MapLocation* loc)
{
    return loc->symbol.sym == '#';
}

static void _make_corridor(struct MapLocation* loc)
{
    loc->symbol.sym = '#';
    loc->pathing_flags = PATHING_GROUND;
    loc->feature = NULL;
}

static bool _is_floor(struct MapLocation* loc)
{
    return !loc->feature && loc->symbol.sym == '.';
}

static void _make_floor(struct MapLocation* loc)
{
    loc->symbol.sym = '.';
    loc->pathing_flags = PATHING_GROUND;
    loc->feature = NULL;
}

static bool _is_potential_room_entrance(struct MapCell* cell, struct MapLocation* loc)
{
    struct MapLocation* left  = map_cell_get_location(cell, loc->x-1, loc->y);
    struct MapLocation* right = map_cell_get_location(cell, loc->x+1, loc->y);
    struct MapLocation* up    = map_cell_get_location(cell, loc->x, loc->y-1);
    struct MapLocation* down  = map_cell_get_location(cell, loc->x, loc->y+1);

    if(_is_horizontal_wall(left) && _is_horizontal_wall(right) && (_is_corridor(up) || _is_corridor(down)))
    {
        return true;
    }

    if(_is_vertical_wall(up) && _is_vertical_wall(down) && (_is_corridor(left) || _is_corridor(right)))
    {
        return true;
    }

    return false;
}

void gen_room(struct MapCell* cell)
{
    int w = random_int(4, 10);
    int h = random_int(4, 8);
    int x = random_int(cell->world_x, cell->world_x+g_map_cell_width-1-w);
    int y = random_int(cell->world_y, cell->world_y+g_map_cell_height-1-h);

    ListNode* rnode;
    list_for_each(&cell->room_list, rnode)
    {
        struct Room* r = rnode->data;
        if((((x >= r->x) && (x <= (r->x + r->w))) || ((r->x >= x) && (r->x <= (x + w)))) &&
           (((y >= r->y) && (y <= (r->y + r->h))) || ((r->y >= y) && (r->y <= (y + h)))))
        {
            return;
        }
    }

    struct Room* room = (struct Room*) malloc(sizeof(struct Room));
    room->x = x;
    room->y = y;
    room->w = w;
    room->h = h;

    list_add(&cell->room_list, room);

    struct MapLocation* loc = NULL;
    struct Feature* wall = feature_look_up_by_id("walv");

    for(int tmp = 0; tmp < room->h; tmp++)
    {
        loc = map_cell_get_location(cell, room->x, room->y+tmp);
        loc->feature = wall;

        loc = map_cell_get_location(cell, room->x+room->w-1, room->y+tmp);
        loc->feature = wall;
    }

    // Draw horizontal walls
    wall = feature_look_up_by_id("walh");
    for(int tmp = 0; tmp < room->w; tmp++)
    {
        loc = map_cell_get_location(cell, room->x+tmp, room->y);
        loc->feature = wall;

        loc = map_cell_get_location(cell, room->x+tmp, room->y+room->h-1);
        loc->feature = wall;
    }

    // Fill in with floor
    for(int tmpx = 1; tmpx < room->w-1; ++tmpx)
    for(int tmpy = 1; tmpy < room->h-1; ++tmpy)
    {
        int floor_col = random_int(20,180);
        loc = map_cell_get_location(cell, room->x+tmpx, room->y+tmpy);
        _make_floor(loc);
        loc->symbol.fg = (struct Colour){floor_col,floor_col,floor_col};
        loc->symbol.bg = ((tmpx+tmpy) % 2 == 0) ? (struct Colour){20,20,20} : (struct Colour){26,26,26};
    }
}

/* Draws a map by drawing square rooms at random locations, and with random dimensions. */
void gen_rooms(struct MapCell* cell)
{
    // Place as many rooms as we can, limited to 200 attempts
    int attempts = 200;
    for(int i = 0; i < attempts; i++)
    {
        gen_room(cell);
    }
}

int gen_rooms_task_func(void* state)
{
    struct MapCellGenState* gen_state = state;

    gen_room(gen_state->cell);

    if(++gen_state->room_attempts > gen_state->room_attempts_max)
        return TASK_STATUS_SUCCESS;
    return TASK_STATUS_EXECUTING;
}

/* Check if a location is a valid starting node for a maze
 * Valid maze starting nodes are solid rock surrounded by 8 other solid rock */
bool _is_maze_snode(struct MapCell* cell, struct MapLocation* loc)
{
    if(!_is_solid_rock(loc) || _is_border_loc(cell, loc))
    {
        return false;
    }

    int x = loc->x;
    int y = loc->y;

    if(_is_solid_rock(map_cell_get_location(cell, x-1, y-1)) && _is_solid_rock(map_cell_get_location(cell, x, y-1)) && _is_solid_rock(map_cell_get_location(cell, x+1, y-1)) &&
       _is_solid_rock(map_cell_get_location(cell, x-1, y))   && _is_solid_rock(map_cell_get_location(cell, x+1, y)) &&
       _is_solid_rock(map_cell_get_location(cell, x-1, y+1)) && _is_solid_rock(map_cell_get_location(cell, x, y+1)) && _is_solid_rock(map_cell_get_location(cell, x+1, y+1)))
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

/* Checks to see if a location is a viable maze node
 * Valid maze nodes need to have only 1 connection to another corridor tile.
 * They can also only have 2 of 4 diagonal squares as corridor tiles so long as those tiles
 * are orthogonal to the single connecting corridor tile.
 */
bool _is_valid_maze_node(struct MapCell* cell, struct MapLocation* loc)
{
    if(!_is_solid_rock(loc) || _is_border_loc(cell, loc))
    {
        return false;
    }

    int conn_count = 0;

    struct MapLocation* tmp = NULL;
    struct MapLocation* conn = NULL;

    tmp = map_cell_get_location(cell, loc->x-1, loc->y);
    if(tmp && _is_corridor(tmp))
    {
        conn_count++;
        conn = tmp;
    }

    tmp = map_cell_get_location(cell, loc->x+1, loc->y);
    if(tmp && _is_corridor(tmp))
    {
        conn_count++;
        conn = tmp;
    }

    tmp = map_cell_get_location(cell, loc->x, loc->y-1);
    if(tmp && _is_corridor(tmp))
    {
        conn_count++;
        conn = tmp;
    }

    tmp = map_cell_get_location(cell, loc->x, loc->y+1);
    if(tmp && _is_corridor(tmp))
    {
        conn_count++;
        conn = tmp;
    }

    if(conn_count > 1)
        return false;

    int xoff = loc->x - conn->x;
    int yoff = loc->y - conn->y;

    if(xoff == -1)
    {
        if(_is_corridor(map_cell_get_location(cell, loc->x-1, loc->y-1)) || _is_corridor(map_cell_get_location(cell, loc->x-1, loc->y+1)))
        {
            return false;
        }
    }
    else if(xoff == 1)
    {
        if(_is_corridor(map_cell_get_location(cell, loc->x+1, loc->y-1)) || _is_corridor(map_cell_get_location(cell, loc->x+1, loc->y+1)))
        {
            return false;
        }
    }
    else if(yoff == 1)
    {
        if(_is_corridor(map_cell_get_location(cell, loc->x-1, loc->y+1)) || _is_corridor(map_cell_get_location(cell, loc->x+1, loc->y+1)))
        {
            return false;
        }
    }
    else
    {
        if(_is_corridor(map_cell_get_location(cell, loc->x-1, loc->y-1)) || _is_corridor(map_cell_get_location(cell, loc->x+1, loc->y-1)))
        {
            return false;
        }
    }

   return true;
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
        case 0:
            xoff = -1; break;
        case 1:
            xoff = 1; break;
        case 2:
            yoff = -1; break;
        case 3:
            yoff= 1; break;
    }

    struct MapLocation* next;
    next = map_cell_get_location(cell, x+xoff, y+yoff);
    if(_is_valid_maze_node(cell, next))
        return next;

    next = map_cell_get_location(cell, x+1, y);
    if(_is_valid_maze_node(cell, next))
        return next;

    next = map_cell_get_location(cell, x-1, y);
    if(_is_valid_maze_node(cell, next))
        return next;

    next = map_cell_get_location(cell, x, y+1);
    if(_is_valid_maze_node(cell, next))
        return next;

    next = map_cell_get_location(cell, x, y-1);
    if(_is_valid_maze_node(cell, next))
        return next;

    return NULL;
}

static void _enlist_orthogonals(struct MapCell* cell, struct MapLocation* loc, List* list)
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
                next = map_cell_get_location(cell, loc->x, loc->y-1);
                break;
            case 2: // Down
                next = map_cell_get_location(cell, loc->x, loc->y+1);
                break;
            case 3: // Left
                next = map_cell_get_location(cell, loc->x-1, loc->y);
                break;
            case 4: // Right
                next = map_cell_get_location(cell, loc->x+1, loc->y);
                break;
        }

        // Switch chosen one to the end so it won't be picked again
        int tmp = ortho_dirs[random_index];
        ortho_dirs[random_index] = ortho_dirs[dir_count];
        ortho_dirs[dir_count] = tmp;
        --dir_count;

        if(next)
            list_add(list, next);
    }
}

void _flood_fill_maze(struct MapCell* cell, struct MapLocation* loc)
{
    List loc_list;
    list_init(&loc_list);

    _enlist_orthogonals(cell, loc, &loc_list);

    do
    {
        // Picking from head is breadth first, and results in long straight corridors
        // Picking from tail is depth first, and results in twisty corridors
        struct MapLocation* next = loc_list.tail->data;
        list_rm(&loc_list, loc_list.tail);

        if(!_is_valid_maze_node(cell, next))
            continue;

        _make_corridor(next);
        _enlist_orthogonals(cell, next, &loc_list);
    }
    while(loc_list.count > 0);
}

/* Checks if a given location is a deadend.
 * Deadends have only one connecting corridor tile */
bool _is_maze_deadend(struct MapCell* cell, struct MapLocation* loc)
{
    if(!_is_corridor(loc) || _is_border_loc(cell, loc))
    {
        return false;
    }

    int x = loc->x;
    int y = loc->y;
    int conn_count = 0;

    if(_is_corridor(map_cell_get_location(cell, x-1, y)) || _is_floor(map_cell_get_location(cell, x-1, y)))
    {
        conn_count++;
    }

    if(_is_corridor(map_cell_get_location(cell, x+1, y)) || _is_floor(map_cell_get_location(cell, x+1, y)))
    {
        conn_count++;
    }

    if(_is_corridor(map_cell_get_location(cell, x, y+1)) || _is_floor(map_cell_get_location(cell, x, y+1)))
    {
        conn_count++;
    }

    if(_is_corridor(map_cell_get_location(cell, x, y-1)) || _is_floor(map_cell_get_location(cell, x, y-1)))
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

/* Picks wall sections on each wall at random until it finds one that has a corridor next to it.
 * It converts that wall section into an entryway into the room.
 * TODO: Generate a door object in the entryway tile */
void _make_doors(struct MapCell* cell)
{
    struct Room* room;

    // TODO: This should equal the size of the perimeter of the maximum sized room, make a constant
    struct MapLocation* connectors[36];

    ListNode* rnode;
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
                if(_is_corridor(map_cell_get_location(cell, x, y-1)))
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
                if(_is_corridor(map_cell_get_location(cell, x, y+1)))
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
                if(_is_corridor(map_cell_get_location(cell, x-1, y)))
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
                if(_is_corridor(map_cell_get_location(cell, x+1, y)))
                {
                    connectors[cidx] = map_cell_get_location(cell, x, y);
                    cidx++;
                }
            }
        }

        int doors_to_make = random_int(1, ((room->w%2) + (room->h%2) + room->w + room->h));
        for(int door_count = 0; door_count < doors_to_make;)
        {
            if(cidx == 0) break;

            int which = random_int(0, cidx-1);

            if(_is_potential_room_entrance(cell, connectors[which]))
            {
                _make_floor(connectors[which]);
                ++door_count;
            }

            // Switch in the last one in the array
            connectors[which] = connectors[cidx-1];
            --cidx;
        }
    }
}

int gen_maze_flood_fill_task_func(void* state)
{
    struct MapCellGenState* gen_state = state;

    // Check for potential maze start nodes
    if(gen_state->tmp_list.count == 0)
    {
        struct MapLocation* loc = _get_maze_snode(gen_state->cell);
        if(!loc)
        {
            return TASK_STATUS_SUCCESS;
        }

        _make_corridor(loc);
        _enlist_orthogonals(gen_state->cell, loc, &gen_state->tmp_list);
    }

    // Picking from head is breadth first, and results in long straight corridors
    // Picking from tail is depth first, and results in twisty corridors
    struct MapLocation* next = gen_state->tmp_list.tail->data;
    list_rm(&gen_state->tmp_list, gen_state->tmp_list.tail);

    if(!_is_valid_maze_node(gen_state->cell, next))
    {
        return TASK_STATUS_EXECUTING;
    }

    _make_corridor(next);

    _enlist_orthogonals(gen_state->cell, next, &gen_state->tmp_list);
    return TASK_STATUS_EXECUTING;
}

// TODO: MAKE THIS NON-COOPERATIVE
int gen_maze_back_fill_deadends_task_func(void* state)
{
    struct MapCellGenState* gen_state = state;

    struct MapLocation* loc = NULL;

    // Check for potential maze deadend nodes
    if(gen_state->tmp_list.count == 0)
    {
        loc = _get_maze_deadend(gen_state->cell);
        if(!loc)
        {
            return TASK_STATUS_SUCCESS;
        }

        list_add(&gen_state->tmp_list, loc);
    }

    loc = gen_state->tmp_list.tail->data;
    list_rm(&gen_state->tmp_list, gen_state->tmp_list.tail);

    _make_solid_rock(loc);

    loc = _get_next_deadend_node(gen_state->cell, loc);
    if(loc)
    {
        list_add(&gen_state->tmp_list, loc);
    }

    return TASK_STATUS_EXECUTING;
}


int gen_maze_task_func(void* state)
{
    struct MapCellGenState* gen_state = state;

    switch(gen_state->stage)
    {
        case GEN_STAGE_FLOOD_FILL_MAZE:
        {
            if(gen_maze_flood_fill_task_func(state) == TASK_STATUS_SUCCESS)
                gen_state->stage = GEN_STAGE_DOORS;
            break;
        }

        case GEN_STAGE_DOORS:
        {
            _make_doors(gen_state->cell);
            gen_state->stage = GEN_STAGE_BACK_FILL_MAZE;
            break;
        }

        case GEN_STAGE_BACK_FILL_MAZE:
        {
           if(gen_maze_back_fill_deadends_task_func(state) == TASK_STATUS_SUCCESS)
           {
               gen_state->stage = GEN_STAGE_FINISHED;
               return TASK_STATUS_SUCCESS;
           }
           break;
        }

        default:
            return TASK_STATUS_FAILED;
    }

    return TASK_STATUS_EXECUTING;
}

/* Generate the maze of corridors, adds entryways into rooms, and fills in the deadends */
void gen_maze(struct MapCell* cell)
{
    struct MapLocation* tmp;

    while((tmp = _get_maze_snode(cell)))
    {
        _make_corridor(tmp);
        _flood_fill_maze(cell, tmp);
    }

    _make_doors(cell);

    while((tmp = _get_maze_deadend(cell)))
    {
        _make_solid_rock(tmp);
        _back_fill_deadends(cell, tmp);
    }
}

static void _gen_open_area(struct MapCell* cell)
{
    int w = random_int(g_map_cell_width/4, g_map_cell_width-1);
    int h = random_int(g_map_cell_height/4, g_map_cell_height-1);
    int x = random_int(cell->world_x, cell->world_x + g_map_cell_width-1-w);
    int y = random_int(cell->world_y, cell->world_y + g_map_cell_height-1-h);

#ifdef DEBUG
    log_format_msg(DEBUG, "Room parameters (x, y, w, h): %d, %d, %d, %d", x, y, w, h);
#endif

    struct Room* room = (struct Room*) malloc(sizeof(struct Room));
    room->x = x;
    room->y = y;
    room->w = w;
    room->h = h;

    list_add(&cell->room_list, room); 

    // Fill in with floor
    for(int tmpx = 0; tmpx < w-1; tmpx++)
    for(int tmpy = 0; tmpy < h-1; tmpy++)
    {
        struct MapLocation* loc = map_cell_get_location(cell, x+tmpx, y+tmpy);
        _make_floor(loc);
        loc->symbol.fg = (struct Colour){0,random_int(20, 180),0};
        loc->symbol.bg = ((tmpx+tmpy) % 2 == 0) ? (struct Colour){10,36,10} : (struct Colour){16, 36, 16};
    }
}

/* Call this to generate a map that contains rooms and a load of connecting corridors */
void gen_map_cell(struct MapCell* cell, enum MapType type)
{
    if(type == MAPTYPE_DUNGEON)
    {
        gen_rooms(cell);
        gen_maze(cell);
    }
    else
    {
        _gen_open_area(cell);
    }
}


// Naively connect cells together orthogonally
static void _connect_cells(struct Map* map)
{
    for(int y = 0; y < map->height; ++y)
    for(int x = 0; x < map->width-1; ++x)
    {
        // Horizontal connections
        int hconns = random_int(1, 10);
#ifdef DEBUG
        log_format_msg(DEBUG, "Generating %d horizontal connections from cell (%d, %d) to cell (%d, %d)", hconns, x, y, x+1, y);
#endif
        for(int hconn = 0; hconn < hconns; ++hconn)
        {
            struct MapCell* cell    = NULL;
            struct MapLocation* loc = NULL;
            int threshold = random_int(0, g_map_cell_height-1);
            int offset = 0;

            // If a cell is a potential room entrace, flip it and finish
            // This has to be done in the case that we're directly in line with a room wall
            // The algorithm would just destroy the wall and keep going

            // Carve a path left
            offset = 0;
            cell = map_get_cell_by_map_coord(map, x, y);
            while((loc = map_cell_get_location_relative(cell, g_map_cell_width-1-offset, threshold)) != NULL)
            {
                // We've hit a corridor or internal room square
                if(_is_corridor(loc) || _is_floor(loc))
                {
                    break;
                }

                if(_is_wall(loc))
                {
                    struct Room* room = map_cell_get_room(cell, loc->x, loc->y);
                    if(room_is_corner(room, loc->x, loc->y))
                    {
                        // Cannot cut into a corner currently. Just try again.
                        --hconn;
                    }

                    _make_floor(loc);
                    break;
                }

                _make_corridor(loc);
                ++offset;
            }


            // Carve a path right
            offset = 0;
            cell = map_get_cell_by_map_coord(map, x+1, y);
            while((loc = map_cell_get_location_relative(cell, offset, threshold)) != NULL)
            {
                // We've hit a corridor or internal room square
                if(_is_corridor(loc) || _is_floor(loc))
                    break;

                if(_is_wall(loc))
                {
                    struct Room* room = map_cell_get_room(cell, loc->x, loc->y);
                    if(room_is_corner(room, loc->x, loc->y))
                    {
                        // Cannot cut into a corner currently. Just try again.
                        --hconn;
                    }

                    _make_floor(loc);
                    break;
                }

                _make_corridor(loc);
                ++offset;
            }
        }

        int vconns = random_int(1, 10);
#ifdef DEBUG
        log_format_msg(DEBUG, "Generating %d vertical connections from cell (%d, %d) to cell (%d, %d)", vconns, y, x, y, x + 1);
#endif
        for(int vconn = 0; vconn < vconns; ++vconn)
        {
            struct MapCell* cell    = NULL;
            struct MapLocation* loc = NULL;
            int threshold = random_int(0, g_map_cell_width-1);
            int offset = 0;

            // Carve a path up
            offset = 0;
            cell = map_get_cell_by_map_coord(map, y, x);
            while((loc = map_cell_get_location_relative(cell, threshold, g_map_cell_height-1-offset)) != NULL)
            {
                // We've hit a corridor or internal room square
                if(_is_corridor(loc) || _is_floor(loc))
                    break;

                if(_is_wall(loc))
                {
                    struct Room* room = map_cell_get_room(cell, loc->x, loc->y);
                    if(room_is_corner(room, loc->x, loc->y))
                    {
                        // Cannot cut into a corner currently. Just try again.
                        --vconn;
                    }

                    _make_floor(loc);
                    break;
                }

                _make_corridor(loc);
                ++offset;
            }


            // Carve a path down
            offset = 0;
            cell = map_get_cell_by_map_coord(map, y, x+1);
            while((loc = map_cell_get_location_relative(cell, threshold, offset)) != NULL)
            {
                // We've hit a corridor or internal room square
                if(_is_corridor(loc) || _is_floor(loc))
                    break;

                if(_is_wall(loc))
                {
                    struct Room* room = map_cell_get_room(cell, loc->x, loc->y);
                    if(room_is_corner(room, loc->x, loc->y))
                    {
                        // Cannot cut into a corner currently. Just try again.
                        --vconn;
                    }

                    _make_floor(loc);
                    break;
                }

                _make_corridor(loc);
                ++offset;
            }
        }
    }
}

static void _gen_rooms_async(struct MapCell* cell)
{
    struct MapCellGenState state;
    state.cell = cell;
    state.room_attempts = 0;
    state.room_attempts_max = 200;
    state.stage = GEN_STAGE_ROOMS;

    struct Task* gen_rooms_task = task_new("Gen rooms", gen_rooms_task_func, NULL, &state, sizeof(state));
    tasker_add_task(g_tasker, gen_rooms_task);

    log_msg(DEBUG, "Added gen room task.");
}

static void _gen_maze_async(struct MapCell* cell)
{
    struct MapCellGenState state;
    state.cell = cell;
    state.stage = GEN_STAGE_FLOOD_FILL_MAZE;
    list_init(&state.tmp_list);

    struct Task* task = task_new("Gen maze", gen_maze_task_func, NULL, &state, sizeof(state));
    tasker_add_task(g_tasker, task);

    log_msg(DEBUG, "Added gen maze task.");
}

void gen_map(struct Map* map, enum MapType type)
{
    ListNode* node = NULL;

    for(int x = 0; x < map->width; ++x)
    for(int y = 0; y < map->height; ++y)
    {
        struct MapCell* cell = map_cell_new(x, y);
        list_add(&map->cell_list, cell);
    }

    int loading_progress_x = (screen_cols/2) - 9;
    int loading_progress_y = screen_rows/8;

    term_draw_text(loading_progress_x, loading_progress_y, NULL, NULL, A_BOLD, "Generating map");
    term_draw_text(loading_progress_x, loading_progress_y + 1, NULL, NULL, 0, "Creating rooms...");
    term_refresh();

    list_for_each(&map->cell_list, node)
    {
        _gen_rooms_async(node->data);
    }
    tasker_sync(g_tasker);
    tasker_integrate(g_tasker);

    term_draw_text(loading_progress_x, loading_progress_y + 1, NULL, NULL, 0, "Creating rooms... Done!");
    term_draw_text(loading_progress_x, loading_progress_y + 2, NULL, NULL, 0, "Generating maze...");
    term_refresh();

    list_for_each(&map->cell_list, node)
    {
        _gen_maze_async(node->data);
    }
    tasker_sync(g_tasker);
    tasker_integrate(g_tasker);

    term_draw_text(loading_progress_x, loading_progress_y + 2, NULL, NULL, 0, "Generating maze... Done!");
    term_draw_text(loading_progress_x, loading_progress_y + 3, NULL, NULL, 0, "Connecting cells...");
    term_refresh();

    if(type == MAPTYPE_DUNGEON)
        _connect_cells(map);

    term_draw_text(loading_progress_x, loading_progress_y + 3, NULL, NULL, 0, "Connecting cells... Done!");
    term_draw_text(loading_progress_x, loading_progress_y + 5, NULL, NULL, 0, "Entering Yun...");
    term_refresh();
    term_wait_on_input();
    term_clear();
}
