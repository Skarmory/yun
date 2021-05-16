#include "map_gen.h"
#include "map_gen_maze.h"
#include "map_gen_utils.h"

#include "colour.h"
#include "feature.h"
#include "globals.h"
#include "log.h"
#include "map.h"
#include "map_cell.h"
#include "map_location.h"
#include "map_room.h"
#include "map_utils.h"
#include "symbol.h"
#include "tasking.h"
#include "util.h"

#include <stdlib.h>
#include <string.h>

// TODO: Check over the maze gen algorithm for performance improvements
// TODO: Ensure that rooms are always connected

// FDECL
static void _gen_room(struct MapCell* cell);
static void _gen_rooms(struct MapCell* cell);
static int  _gen_rooms_task_func(void* state);
static void _gen_open_area(struct MapCell* cell);
static void _gen_rooms_async(struct MapCell* cell);

static void _gen_room(struct MapCell* cell)
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
        map_gen_util_make_floor(loc);
        loc->symbol.fg = (struct Colour){floor_col,floor_col,floor_col};
        loc->symbol.bg = ((tmpx+tmpy) % 2 == 0) ? (struct Colour){20,20,20} : (struct Colour){26,26,26};
    }
}

/* Draws a map by drawing square rooms at random locations, and with random dimensions. */
static void _gen_rooms(struct MapCell* cell)
{
    // Place as many rooms as we can, limited to 200 attempts
    int attempts = 200;
    for(int i = 0; i < attempts; i++)
    {
        _gen_room(cell);
    }
}

static int _gen_rooms_task_func(void* state)
{
    struct MapCell* cell = *(struct MapCell**)state;
    _gen_rooms(cell);
    return TASK_STATUS_SUCCESS;
}

static void _gen_open_area(struct MapCell* cell)
{
    int w = random_int(g_map_cell_width/4, g_map_cell_width-1);
    int h = random_int(g_map_cell_height/4, g_map_cell_height-1);
    int x = random_int(cell->world_x, cell->world_x + g_map_cell_width-1-w);
    int y = random_int(cell->world_y, cell->world_y + g_map_cell_height-1-h);

#ifdef LOG_DEBUG
    log_format_msg(LOG_DEBUG, "Room parameters (x, y, w, h): %d, %d, %d, %d", x, y, w, h);
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
        map_gen_util_make_floor(loc);
        loc->symbol.fg = (struct Colour){0,random_int(20, 180),0};
        loc->symbol.bg = ((tmpx+tmpy) % 2 == 0) ? (struct Colour){10,36,10} : (struct Colour){16, 36, 16};
    }
}

static void _gen_rooms_async(struct MapCell* cell)
{
    struct Task* gen_rooms_task = task_new("Gen rooms", _gen_rooms_task_func, NULL, &cell, sizeof(cell));
    tasker_add_task(g_tasker, gen_rooms_task);

    log_msg(LOG_DEBUG, "Added gen rooms task.");
}

// Naively connect cells together orthogonally
static void _connect_cells(struct Map* map)
{
    for(int y = 0; y < map->height; ++y)
    for(int x = 0; x < map->width-1; ++x)
    {
        // Horizontal connections
        int hconns = random_int(1, 10);
#ifdef LOG_DEBUG
        log_format_msg(LOG_DEBUG, "Generating %d horizontal connections from cell (%d, %d) to cell (%d, %d)", hconns, x, y, x+1, y);
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
                if(map_util_is_corridor(loc) || map_util_is_floor(loc))
                {
                    break;
                }

                if(map_util_is_wall(loc))
                {
                    struct Room* room = map_cell_get_room(cell, loc->x, loc->y);
                    if(room_is_corner(room, loc->x, loc->y))
                    {
                        // Cannot cut into a corner currently. Just try again.
                        --hconn;
                    }

                    map_gen_util_make_floor(loc);
                    break;
                }

                map_gen_util_make_corridor(loc);
                ++offset;
            }


            // Carve a path right
            offset = 0;
            cell = map_get_cell_by_map_coord(map, x+1, y);
            while((loc = map_cell_get_location_relative(cell, offset, threshold)) != NULL)
            {
                // We've hit a corridor or internal room square
                if(map_util_is_corridor(loc) || map_util_is_floor(loc))
                {
                    break;
                }

                if(map_util_is_wall(loc))
                {
                    struct Room* room = map_cell_get_room(cell, loc->x, loc->y);
                    if(room_is_corner(room, loc->x, loc->y))
                    {
                        // Cannot cut into a corner currently. Just try again.
                        --hconn;
                    }

                    map_gen_util_make_floor(loc);
                    break;
                }

                map_gen_util_make_corridor(loc);
                ++offset;
            }
        }

        int vconns = random_int(1, 10);
#ifdef LOG_DEBUG
        log_format_msg(LOG_DEBUG, "Generating %d vertical connections from cell (%d, %d) to cell (%d, %d)", vconns, y, x, y, x + 1);
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
                if(map_util_is_corridor(loc) || map_util_is_floor(loc))
                {
                    break;
                }

                if(map_util_is_wall(loc))
                {
                    struct Room* room = map_cell_get_room(cell, loc->x, loc->y);
                    if(room_is_corner(room, loc->x, loc->y))
                    {
                        // Cannot cut into a corner currently. Just try again.
                        --vconn;
                    }

                    map_gen_util_make_floor(loc);
                    break;
                }

                map_gen_util_make_corridor(loc);
                ++offset;
            }


            // Carve a path down
            offset = 0;
            cell = map_get_cell_by_map_coord(map, y, x+1);
            while((loc = map_cell_get_location_relative(cell, threshold, offset)) != NULL)
            {
                // We've hit a corridor or internal room square
                if(map_util_is_corridor(loc) || map_util_is_floor(loc))
                {
                    break;
                }

                if(map_util_is_wall(loc))
                {
                    struct Room* room = map_cell_get_room(cell, loc->x, loc->y);
                    if(room_is_corner(room, loc->x, loc->y))
                    {
                        // Cannot cut into a corner currently. Just try again.
                        --vconn;
                    }

                    map_gen_util_make_floor(loc);
                    break;
                }

                map_gen_util_make_corridor(loc);
                ++offset;
            }
        }
    }
}

static void _gen_map_dungeon(struct Map* map)
{
    int loading_progress_x = (screen_cols/2) - 9;
    int loading_progress_y = screen_rows/8;

    term_draw_text(loading_progress_x, loading_progress_y, NULL, NULL, A_BOLD, "Generating map");
    term_draw_text(loading_progress_x, loading_progress_y + 1, NULL, NULL, 0, "Creating rooms...");
    term_refresh();

    ListNode* node = NULL;
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
        map_gen_maze_async(node->data);
    }
    tasker_sync(g_tasker);
    tasker_integrate(g_tasker);

    term_draw_text(loading_progress_x, loading_progress_y + 2, NULL, NULL, 0, "Generating maze... Done!");
    term_draw_text(loading_progress_x, loading_progress_y + 3, NULL, NULL, 0, "Connecting cells...");
    term_refresh();

    _connect_cells(map);

    term_draw_text(loading_progress_x, loading_progress_y + 3, NULL, NULL, 0, "Connecting cells... Done!");
    term_draw_text(loading_progress_x, loading_progress_y + 5, NULL, NULL, 0, "Entering Yun...");
    term_refresh();
    term_wait_on_input();
    term_clear();
}

static void _gen_map_open(struct Map* map)
{
    ListNode* node = NULL;
    list_for_each(&map->cell_list, node)
    {
        _gen_open_area(node->data);
    }
}

void gen_map(struct Map* map, enum MapType type)
{
    for(int x = 0; x < map->width; ++x)
    for(int y = 0; y < map->height; ++y)
    {
        struct MapCell* cell = map_cell_new(x, y);
        list_add(&map->cell_list, cell);
    }

    switch(type)
    {
        case MAPTYPE_DUNGEON:
        {
            _gen_map_dungeon(map);
            break;
        }
        case MAPTYPE_OPEN:
        {
            _gen_map_open(map);
            break;
        }
    }
}
