#include "map_gen.h"

#include "components/gfx_terminal_component.h"
#include "components/map_component.h"

#include "entities.h"
#include "feature.h"
#include "globals.h"
#include "map.h"
#include "map_cell.h"
#include "map_gen_maze.h"
#include "map_gen_utils.h"
#include "map_location.h"
#include "map_room.h"
#include "map_utils.h"
#include "terrain.h"
#include "util.h"

#include <scieppend/core/colour.h>
#include <scieppend/core/ecs.h>
#include <scieppend/core/log.h>
#include <scieppend/core/stack_array.h>
#include <scieppend/core/symbol.h>
#include <scieppend/core/tasking.h>
#include <scieppend/core/term.h>

#include <stdlib.h>
#include <string.h>

// TODO: Check over the maze gen algorithm for performance improvements
// TODO: Ensure that rooms are always connected

// FDECL
static void _gen_room(struct MapCell* cell);
static void _gen_rooms(struct MapCell* cell);
static int  _gen_rooms_task_func(void* state);
static void _gen_open_area(struct MapCell* cell);
static struct Task* _gen_rooms_async(struct MapCell* cell);

static const struct Feature* s_wallv = NULL;
static const struct Feature* s_wallh = NULL;
static const struct Terrain* s_floor = NULL;

static EntityHandle _create_feature(int x, int y, const struct Feature* feature)
{
    EntityHandle e_h = entity_create();
    add_position_component(e_h, x, y, 1);
    add_gfx_component(e_h, feature->symbol);
    add_feature_component(e_h, feature->id_hash);
    return e_h;
}

static EntityHandle _create_terrain(int x, int y, const struct Terrain* terrain)
{
    EntityHandle e_h = entity_create();
    add_position_component(e_h, x, y, 0);
    add_gfx_component(e_h, terrain->symbol);
    add_terrain_component(e_h, terrain->id_hash);
    return e_h;
}

static void _gen_room(struct MapCell* cell)
{
    int w = random_int(4, 10);
    int h = random_int(4, 8);
    int x = random_int(cell->world_x, cell->world_x+g_map_cell_width-1-w);
    int y = random_int(cell->world_y, cell->world_y+g_map_cell_height-1-h);

    for(int i = 0; i < array_count(&cell->rooms); ++i)
    {
        struct Room* r = array_get(&cell->rooms, i);
        if((((x >= r->x) && (x <= (r->x + r->w))) || ((r->x >= x) && (r->x <= (x + w)))) &&
           (((y >= r->y) && (y <= (r->y + r->h))) || ((r->y >= y) && (r->y <= (y + h)))))
        {
            return;
        }
    }

    struct Room room;
    room.x = x;
    room.y = y;
    room.w = w;
    room.h = h;

    array_add(&cell->rooms, &room);

    struct MapLocation* loc = NULL;

    // Draw vertical walls
    for(int yoff = 0; yoff < room.h; ++yoff)
    {
        loc = map_cell_get_location(cell, room.x, room.y + yoff);
        loc->feature = _create_feature(loc->x, loc->y, s_wallv);
        loc = map_cell_get_location(cell, room.x + room.w - 1, room.y + yoff);
        loc->feature = _create_feature(loc->x, loc->y, s_wallv);
    }

    // Draw horizontal walls
    for(int xoff = 0; xoff < room.w; ++xoff)
    {
        loc = map_cell_get_location(cell, room.x + xoff, room.y);
        loc->feature = _create_feature(loc->x, loc->y, s_wallh);
        loc = map_cell_get_location(cell, room.x + xoff, room.y + room.h - 1);
        loc->feature = _create_feature(loc->x, loc->y, s_wallh);
    }

    // Fill in with floor
    for(int xoff = 0; xoff < room.w; ++xoff)
    for(int yoff = 0; yoff < room.h; ++yoff)
    {
        int floor_col = random_int(20, 180);
        loc = map_cell_get_location(cell, room.x + xoff, room.y + yoff);
        loc->terrain = _create_terrain(loc->x, loc->y, s_floor);

        struct GFXTerminalComponent* ter_c = entity_get_component(loc->terrain, g_GFXTerminalComponent_id);

        ter_c->symbol.fg = (struct Colour){floor_col, floor_col, floor_col};
        ter_c->symbol.bg = ((xoff + yoff) % 2 == 0) ? (struct Colour){ 20, 20, 20 } : (struct Colour){ 26, 26, 26 };
        
        entity_unget_component(loc->terrain, g_GFXTerminalComponent_id);
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

//static void _gen_open_area(struct MapCell* cell)
//{
//    int w = random_int(g_map_cell_width/4, g_map_cell_width-1);
//    int h = random_int(g_map_cell_height/4, g_map_cell_height-1);
//    int x = random_int(cell->world_x, cell->world_x + g_map_cell_width-1-w);
//    int y = random_int(cell->world_y, cell->world_y + g_map_cell_height-1-h);
//
//#ifdef LOG_DEBUG
//    log_format_msg(LOG_DEBUG, "Room parameters (x, y, w, h): %d, %d, %d, %d", x, y, w, h);
//#endif
//
//    struct Room* room = (struct Room*) malloc(sizeof(struct Room));
//    room->x = x;
//    room->y = y;
//    room->w = w;
//    room->h = h;
//
//    list_add(&cell->room_list, room);
//
//    // Fill in with floor
//    for(int tmpx = 0; tmpx < w-1; tmpx++)
//    for(int tmpy = 0; tmpy < h-1; tmpy++)
//    {
//        struct MapLocation* loc = map_cell_get_location(cell, x+tmpx, y+tmpy);
//        map_gen_util_make_floor(loc);
//        loc->symbol.fg = (struct Colour){0,random_int(20, 180),0};
//        loc->symbol.bg = ((tmpx+tmpy) % 2 == 0) ? (struct Colour){10,36,10} : (struct Colour){16, 36, 16};
//    }
//}

static struct Task* _gen_rooms_async(struct MapCell* cell)
{
    struct Task* gen_rooms_task = task_new("Gen rooms", &_gen_rooms_task_func, NULL, &cell, sizeof(cell));
    tasker_add_task(g_tasker, gen_rooms_task);

    log_msg(LOG_DEBUG, "Added gen rooms task.");
    return gen_rooms_task;
}

// Naively connect cells together orthogonally
//static void _connect_cells(struct Map* map)
//{
//    for(int y = 0; y < map->height; ++y)
//    for(int x = 0; x < map->width-1; ++x)
//    {
//        // Horizontal connections
//        int hconns = random_int(1, 10);
//#ifdef LOG_DEBUG
//        log_format_msg(LOG_DEBUG, "Generating %d horizontal connections from cell (%d, %d) to cell (%d, %d)", hconns, x, y, x+1, y);
//#endif
//        for(int hconn = 0; hconn < hconns; ++hconn)
//        {
//            struct MapCell* cell    = NULL;
//            struct MapLocation* loc = NULL;
//            int threshold = random_int(0, g_map_cell_height-1);
//            int offset = 0;
//
//            // If a cell is a potential room entrace, flip it and finish
//            // This has to be done in the case that we're directly in line with a room wall
//            // The algorithm would just destroy the wall and keep going
//
//            // Carve a path left
//            offset = 0;
//            cell = map_get_cell_by_map_coord(map, x, y);
//            while((loc = map_cell_get_location_relative(cell, g_map_cell_width-1-offset, threshold)) != NULL)
//            {
//                // We've hit a corridor or internal room square
//                if(map_util_is_corridor(loc) || map_util_is_floor(loc))
//                {
//                    break;
//                }
//
//                if(map_util_is_wall(loc))
//                {
//                    struct Room* room = map_cell_get_room(cell, loc->x, loc->y);
//                    if(room && room_is_corner(room, loc->x, loc->y))
//                    {
//                        // Cannot cut into a corner currently. Just try again.
//                        --hconn;
//                    }
//
//                    map_gen_util_make_floor(loc);
//                    break;
//                }
//
//                map_gen_util_make_corridor(loc);
//                ++offset;
//            }
//
//
//            // Carve a path right
//            offset = 0;
//            cell = map_get_cell_by_map_coord(map, x+1, y);
//            while((loc = map_cell_get_location_relative(cell, offset, threshold)) != NULL)
//            {
//                // We've hit a corridor or internal room square
//                if(map_util_is_corridor(loc) || map_util_is_floor(loc))
//                {
//                    break;
//                }
//
//                if(map_util_is_wall(loc))
//                {
//                    struct Room* room = map_cell_get_room(cell, loc->x, loc->y);
//                    if(room_is_corner(room, loc->x, loc->y))
//                    {
//                        // Cannot cut into a corner currently. Just try again.
//                        --hconn;
//                    }
//
//                    map_gen_util_make_floor(loc);
//                    break;
//                }
//
//                map_gen_util_make_corridor(loc);
//                ++offset;
//            }
//        }
//
//        int vconns = random_int(1, 10);
//#ifdef LOG_DEBUG
//        log_format_msg(LOG_DEBUG, "Generating %d vertical connections from cell (%d, %d) to cell (%d, %d)", vconns, y, x, y, x + 1);
//#endif
//        for(int vconn = 0; vconn < vconns; ++vconn)
//        {
//            struct MapCell* cell    = NULL;
//            struct MapLocation* loc = NULL;
//            int threshold = random_int(0, g_map_cell_width-1);
//            int offset = 0;
//
//            // Carve a path up
//            offset = 0;
//            cell = map_get_cell_by_map_coord(map, y, x);
//            while((loc = map_cell_get_location_relative(cell, threshold, g_map_cell_height-1-offset)) != NULL)
//            {
//                // We've hit a corridor or internal room square
//                if(map_util_is_corridor(loc) || map_util_is_floor(loc))
//                {
//                    break;
//                }
//
//                if(map_util_is_wall(loc))
//                {
//                    struct Room* room = map_cell_get_room(cell, loc->x, loc->y);
//                    if(room_is_corner(room, loc->x, loc->y))
//                    {
//                        // Cannot cut into a corner currently. Just try again.
//                        --vconn;
//                    }
//
//                    map_gen_util_make_floor(loc);
//                    break;
//                }
//
//                map_gen_util_make_corridor(loc);
//                ++offset;
//            }
//
//
//            // Carve a path down
//            offset = 0;
//            cell = map_get_cell_by_map_coord(map, y, x+1);
//            while((loc = map_cell_get_location_relative(cell, threshold, offset)) != NULL)
//            {
//                // We've hit a corridor or internal room square
//                if(map_util_is_corridor(loc) || map_util_is_floor(loc))
//                {
//                    break;
//                }
//
//                if(map_util_is_wall(loc))
//                {
//                    struct Room* room = map_cell_get_room(cell, loc->x, loc->y);
//                    if(room_is_corner(room, loc->x, loc->y))
//                    {
//                        // Cannot cut into a corner currently. Just try again.
//                        --vconn;
//                    }
//
//                    map_gen_util_make_floor(loc);
//                    break;
//                }
//
//                map_gen_util_make_corridor(loc);
//                ++offset;
//            }
//        }
//    }
//}

//static void _gen_map_dungeon(struct Map* map)
//{
//    int loading_progress_x = (screen_cols/2) - 9;
//    int loading_progress_y = screen_rows/8;
//
//    term_draw_text(loading_progress_x, loading_progress_y, COL(CLR_DEFAULT), COL(CLR_DEFAULT), A_BOLD_BIT, "Generating map");
//    term_draw_text(loading_progress_x, loading_progress_y + 1, COL(CLR_DEFAULT), COL(CLR_DEFAULT), 0, "Creating rooms...");
//
//    term_refresh();
//
//    StackArray(struct Task*, 32, tasks);
//
//    struct ListNode* node = NULL;
//    list_for_each(&map->cell_list, node)
//    {
//        struct Task* gen_room_task = _gen_rooms_async(node->data);
//        stackarray_add(&tasks, gen_room_task);
//    }
//
//    tasker_sync(g_tasker);
//    for(int i = 0; i < tasks.count; ++i)
//    {
//        task_free(stackarray_get(&tasks, i));
//    }
//    stackarray_clear(&tasks);
//
//    term_draw_text(loading_progress_x, loading_progress_y + 1, COL(CLR_DEFAULT), COL(CLR_DEFAULT), 0, "Creating rooms... Done!");
//    term_draw_text(loading_progress_x, loading_progress_y + 2, COL(CLR_DEFAULT), COL(CLR_DEFAULT), 0, "Generating maze...");
//
//    term_refresh();
//
//    list_for_each(&map->cell_list, node)
//    {
//        struct Task* gen_maze_task = map_gen_maze_async(node->data);
//        stackarray_add(&tasks, gen_maze_task);
//    }
//
//    tasker_sync(g_tasker);
//
//    for(int i = 0; i < tasks.count; ++i)
//    {
//        task_free(stackarray_get(&tasks, i));
//    }
//
//    term_draw_text(loading_progress_x, loading_progress_y + 2, COL(CLR_DEFAULT), COL(CLR_DEFAULT), 0, "Generating maze... Done!");
//    term_draw_text(loading_progress_x, loading_progress_y + 3, COL(CLR_DEFAULT), COL(CLR_DEFAULT), 0, "Connecting cells...");
//
//    term_refresh();
//
//    _connect_cells(map);
//
//    term_draw_text(loading_progress_x, loading_progress_y + 3, COL(CLR_DEFAULT), COL(CLR_DEFAULT), 0, "Connecting cells... Done!");
//    term_draw_text(loading_progress_x, loading_progress_y + 5, COL(CLR_DEFAULT), COL(CLR_DEFAULT), 0, "Entering Yun...");
//
//    term_refresh();
//    term_wait_on_input();
//    term_clear();
//}

//static void _gen_map_open(struct Map* map)
//{
//    struct ListNode* node = NULL;
//    list_for_each(&map->cell_list, node)
//    {
//        _gen_open_area(node->data);
//    }
//}

//void gen_map(struct Map* map, enum MapType type)
//{
//    for(int x = 0; x < map->width; ++x)
//    for(int y = 0; y < map->height; ++y)
//    {
//        struct MapCell* cell = map_cell_new(x, y);
//        list_add(&map->cell_list, cell);
//    }
//
//    switch(type)
//    {
//        case MAPTYPE_DUNGEON:
//        {
//            _gen_map_dungeon(map);
//            break;
//        }
//        case MAPTYPE_OPEN:
//        {
//            _gen_map_open(map);
//            break;
//        }
//    }
//}

static void _generate_map_rooms(struct MapComponent* map)
{
    int loading_progress_x = (screen_cols/2) - 9;
    int loading_progress_y = screen_rows/8;

    term_draw_text(loading_progress_x, loading_progress_y, COL(CLR_DEFAULT), COL(CLR_DEFAULT), A_BOLD_BIT, "Generating map");
    term_draw_text(loading_progress_x, loading_progress_y + 1, COL(CLR_DEFAULT), COL(CLR_DEFAULT), 0, "Creating rooms...");

    term_refresh();

    StackArray(struct Task*, 9) tasks;
    stackarray_init(&tasks);

    for(int i = 0; i < array_count(&map->cells); ++i)
    {
        struct MapCell* cell = array_get(&map->cells, i);
        struct Task* gen_room_task = _gen_rooms_async(cell);
        stackarray_add(&tasks, gen_room_task);
    }

    tasker_sync(g_tasker);

    for(int i = 0; i < tasks.count; ++i)
    {
        task_free(stackarray_get(&tasks, i));
    }
}

static void _generate_maze(struct MapComponent* map)
{
    int loading_progress_x = (screen_cols/2) - 9;
    int loading_progress_y = screen_rows/8;

    term_draw_text(loading_progress_x, loading_progress_y + 1, COL(CLR_DEFAULT), COL(CLR_DEFAULT), 0, "Creating rooms... Done!");
    term_draw_text(loading_progress_x, loading_progress_y + 2, COL(CLR_DEFAULT), COL(CLR_DEFAULT), 0, "Generating maze...");

    term_refresh();

    StackArray(struct Task*, 9) tasks;
    stackarray_init(&tasks);

    for(int i = 0; i < array_count(&map->cells); ++i)
    {
        struct MapCell* cell = array_get(&map->cells, i);
        struct Task* gen_maze_task = map_gen_maze_async(cell);
        stackarray_add(&tasks, gen_maze_task);
    }

    tasker_sync(g_tasker);

    for(int i = 0; i < tasks.count; ++i)
    {
        task_free(stackarray_get(&tasks, i));
    }
}

void generate_map(void)
{
    s_wallv = feature_look_up_by_id("walv");
    s_wallh = feature_look_up_by_id("walh");
    s_floor  = terrain_look_up_by_id("flor");

    EntityHandle map = entity_create();
    ComponentHandle map_h = entity_add_component(map, g_MapComponent_id);

    const int map_width = 1;
    const int map_height = 1;

    struct MapComponent* map_c = component_get(map_h, g_MapComponent_id);
    map_component_init(map_c, map_width, map_height);

    for(int i = 0; i < map_width; ++i)
    for(int j = 0; j < map_height; ++j)
    {
        struct MapCell cell;
        map_cell_init(&cell, i, j);
        array_add(&map_c->cells, &cell);
    }

    _generate_map_rooms(map_c);
    _generate_maze(map_c);

    component_unget(map_h, g_MapComponent_id);
}
