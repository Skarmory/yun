#include "map_gen/map_gen.h"

#include "components/feature_component.h"
#include "components/gfx_component.h"
#include "components/gfx_terminal_component.h"
#include "components/map_component.h"
#include "components/position_component.h"
#include "components/terrain_component.h"

#include "feature.h"
#include "game_world.h"
#include "globals.h"
#include "map.h"
#include "map_cell.h"
#include "map_gen/map_gen_cell.h"
#include "map_gen/map_gen_location.h"
#include "map_gen/map_gen_maze.h"
#include "map_gen/map_gen_room.h"
#include "map_gen/map_gen_utils.h"
#include "map_location.h"
#include "map_room.h"
#include "terrain.h"
#include "util.h"

#include <scieppend/core/colour.h>
#include <scieppend/core/ecs_world.h>
#include <scieppend/core/hash.h>
#include <scieppend/core/log.h>
#include <scieppend/core/rng.h>
#include <scieppend/core/stack_array.h>
#include <scieppend/core/symbol.h>
#include <scieppend/core/tasking.h>
#include <scieppend/core/term.h>

#include <stdlib.h>
#include <string.h>

// TODO: Check over the maze gen algorithm for performance improvements
// TODO: Ensure that rooms are always connected

static void _init_cell(struct MapGenCell* cell)
{
    const struct Terrain* floor = terrain_look_up_by_id("flor");
    const struct Feature* rock = feature_look_up_by_id("sroc");

    for(int x = 0; x < g_map_cell_width; ++x)
    for(int y = 0; y < g_map_cell_height; ++y)
    {
        struct MapGenLocation* loc = map_gen_cell_get_location_relative(cell, x, y);
        loc->feature = rock;
        loc->terrain = floor;
    }
}

static int _gen_cell_task_func(void* state)
{
    //struct MapCell* cell = *(struct MapCell**)state;
    struct MapGenCell* cell = *(struct MapGenCell**)state;
    _init_cell(cell);
    map_gen_rooms(cell);
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

//static struct Task* _gen_cell_async(struct MapCell* cell)
static struct Task* _gen_cell_async(struct MapGenCell* cell)
{
    struct Task* gen_rooms_task = task_new("Gen rooms task", &_gen_cell_task_func, NULL, &cell, sizeof(cell));
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
//        struct Task* _gen_cell_task = _gen_cell_async(node->data);
//        stackarray_add(&tasks, _gen_cell_task);
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

static void _generate_map_cells(struct Array* cells)
{
    int loading_progress_x = (screen_cols / 2) - 9;
    int loading_progress_y = screen_rows / 8;

    term_draw_text(loading_progress_x, loading_progress_y, COL(CLR_DEFAULT), COL(CLR_DEFAULT), A_BOLD_BIT, "Generating map");
    term_draw_text(loading_progress_x, loading_progress_y + 1, COL(CLR_DEFAULT), COL(CLR_DEFAULT), 0, "Creating rooms...");
    term_refresh();

    StackArray(struct Task*, 9) tasks;
    stackarray_init(&tasks);

    for(int i = 0; i < array_count(cells); ++i)
    {
        struct MapGenCell* cell = array_get(cells, i);
        struct Task* _gen_cell_task = _gen_cell_async(cell);
        stackarray_add(&tasks, _gen_cell_task);
    }

    tasker_sync(g_tasker);

    for(int i = 0; i < tasks.count; ++i)
    {
        task_free(stackarray_get(&tasks, i));
    }
}

static void _generate_map_maze(struct Array* cells)
{
    int loading_progress_x = (screen_cols / 2) - 9;
    int loading_progress_y = screen_rows / 8;

    term_draw_text(loading_progress_x, loading_progress_y + 2, COL(CLR_DEFAULT), COL(CLR_DEFAULT), 0, "Creating maze...");
    term_refresh();

    StackArray(struct Task*, 9) tasks;
    stackarray_init(&tasks);

    for(int i = 0; i < array_count(cells); ++i)
    {
        struct MapGenCell* cell = array_get(cells, i);
        struct Task* _gen_maze_task = map_gen_maze_async(cell);
        stackarray_add(&tasks, _gen_maze_task);
    }

    tasker_sync(g_tasker);

    for(int i = 0; i < tasks.count; ++i)
    {
        task_free(stackarray_get(&tasks, i));
    }
}

static void _instantiate_map_location(struct MapGenLocation* mg_loc, struct MapLocation* loc)
{
    struct ECSWorld* world = game_world_get();

    loc->terrain = ecs_world_create_entity(world);
    add_position_component(loc->terrain, mg_loc->x, mg_loc->y, 0);
    add_terrain_component(loc->terrain, mg_loc->terrain->id_hash);
    add_gfx_terminal_component(loc->terrain, mg_loc->terrain->symbol);

    if(mg_loc->feature != NULL)
    {
        loc->feature = ecs_world_create_entity(game_world_get());
        add_position_component(loc->feature, mg_loc->x, mg_loc->y, 1);
        add_feature_component(loc->feature, mg_loc->feature->id_hash);
        add_gfx_terminal_component(loc->feature, mg_loc->feature->symbol);
    }
}

static void _instantiate_map_rooms(struct MapGenCell* mg_cell, struct MapCell* cell)
{
    struct ECSWorld* world = game_world_get();

    for(int ridx = 0; ridx < array_count(&mg_cell->rooms); ++ridx)
    {
        struct Room* room = array_get(&mg_cell->rooms, ridx);

        for(int x = room->x; x < (room->x + room->w); ++x)
        for(int y = room->y; y < (room->y + room->h); ++y)
        {
            struct MapLocation* loc = map_cell_get_location(cell, x, y);

            struct GFXTerminalComponent* gfx_c = ecs_world_entity_get_component(world, loc->terrain, COMPONENT_TYPE_ID(GFXTerminalComponent), WRITE);
            {
                if(room_is_floor(room, x, y))
                {
                    int floor_col = rng_range(mg_cell->rng, 20, 180);
                    gfx_c->symbol.fg = (struct Colour){ floor_col, floor_col, floor_col };
                    gfx_c->symbol.bg = ((x + y) % 2 == 0) ? (struct Colour){ 20, 20, 20 } : (struct Colour){ 26, 26, 26 };
                }
            }
            ecs_world_entity_unget_component(world, loc->terrain, COMPONENT_TYPE_ID(GFXTerminalComponent), WRITE);
        }
    }
}

static void _instantiate_map(struct Array* cells)
{
    int loading_progress_x = (screen_cols / 2) - 9;
    int loading_progress_y = screen_rows / 8;
    term_draw_text(loading_progress_x, loading_progress_y + 3, COL(CLR_DEFAULT), COL(CLR_DEFAULT), 0, "Instantiating entities...");
    term_refresh();

    EntityHandle map_eh = ecs_world_create_entity(game_world_get());
    ecs_world_entity_add_component(game_world_get(), map_eh, COMPONENT_TYPE_ID(MapComponent));

    const int map_width = 1;
    const int map_height = 1;

    struct MapComponent* map_c = ecs_world_entity_get_component(game_world_get(), map_eh, COMPONENT_TYPE_ID(MapComponent), WRITE);
    map_component_init(map_c, map_width, map_height);

    for(int i = 0; i < array_count(cells); ++i)
    {
        struct MapGenCell* mapgen_cell = array_get(cells, i);
        struct MapCell cell;
        map_cell_init(&cell, mapgen_cell->cell_x, mapgen_cell->cell_y);

        for(int x = 0; x < g_map_cell_width; ++x)
        for(int y = 0; y < g_map_cell_height; ++y)
        {
            struct MapGenLocation* mapgen_loc = map_gen_cell_get_location_relative(mapgen_cell, x, y);
            struct MapLocation* loc = map_cell_get_location_relative(&cell, x, y);
            _instantiate_map_location(mapgen_loc, loc);
        }

        _instantiate_map_rooms(mapgen_cell, &cell);

        array_add(&map_c->cells, &cell);
    }

    ecs_world_entity_unget_component(game_world_get(), map_eh, COMPONENT_TYPE_ID(MapComponent), WRITE);

    term_clear();
}

//static void _generate_maze(struct MapComponent* map)
//{
//    int loading_progress_x = (screen_cols/2) - 9;
//    int loading_progress_y = screen_rows/8;
//
//    term_draw_text(loading_progress_x, loading_progress_y + 1, COL(CLR_DEFAULT), COL(CLR_DEFAULT), 0, "Creating rooms... Done!");
//    term_draw_text(loading_progress_x, loading_progress_y + 2, COL(CLR_DEFAULT), COL(CLR_DEFAULT), 0, "Generating maze...");
//
//    term_refresh();
//
//    StackArray(struct Task*, 9) tasks;
//    stackarray_init(&tasks);
//
//    for(int i = 0; i < array_count(&map->cells); ++i)
//    {
//        struct MapCell* cell = array_get(&map->cells, i);
//        struct Task* gen_maze_task = map_gen_maze_async(cell);
//        stackarray_add(&tasks, gen_maze_task);
//    }
//
//    tasker_sync(g_tasker);
//
//    for(int i = 0; i < tasks.count; ++i)
//    {
//        task_free(stackarray_get(&tasks, i));
//    }
//}

void generate_map(void)
{
    struct RNG* mapgen_base_rng = rng_new(7);

    const int map_width = 1;
    const int map_height = 1;

    struct Array cells;
    array_init(&cells, sizeof(struct MapGenCell), (map_width * map_height), NULL, NULL);

    for(int i = 0; i < map_width; ++i)
    for(int j = 0; j < map_height; ++j)
    {
        struct MapGenCell cell;
        map_gen_cell_init(&cell, i, j, rng_get(mapgen_base_rng));
        array_add(&cells, &cell);
    }

    _generate_map_cells(&cells);
    _generate_map_maze(&cells);
    _instantiate_map(&cells);

    for(int i = 0; i < array_count(&cells); ++i)
    {
        struct MapGenCell* cell = array_get(&cells, i);
        map_gen_cell_uninit(cell);
    }

    array_uninit(&cells);
}
