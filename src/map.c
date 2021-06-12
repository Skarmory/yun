#include "map.h"

#include "colour.h"
#include "feature.h"
#include "gameplay_commands.h"
#include "input_keycodes.h"
#include "log.h"
#include "map_cell.h"
#include "map_location.h"
#include "monster.h"
#include "mon_type.h"
#include "object.h"
#include "player.h"
#include "pathing.h"
#include "symbol.h"
#include "term.h"
#include "util.h"

#include <stdlib.h>

struct Map* g_cmap = NULL;

/**
 * Creates the map and sets map locations to default values
 */
struct Map* map_new(int width, int height)
{
    struct Map* map = malloc(sizeof(struct Map));
    map->width = width;
    map->height = height;
    list_init(&map->cell_list);

    return map;
}

/**
 * Safely deletes the map
 */
void map_free(struct Map* map)
{
    struct ListNode *node = NULL, *next = NULL;
    list_for_each_safe(&map->cell_list, node, next)
    {
        map_cell_free(node->data);
        free(node);
    }

    free(map);
}

struct MapCell* map_get_cell_by_world_coord(struct Map* map, int x, int y)
{
    struct ListNode* node = NULL;
    list_for_each(&map->cell_list, node)
    {
        if(map_cell_is_in_bounds(node->data, x, y))
        {
            return node->data;
        }
    }

    return NULL;
}

struct MapCell* map_get_cell_by_map_coord(struct Map* map, int x, int y)
{
    struct ListNode* node = NULL;
    list_for_each(&map->cell_list, node)
    {
        struct MapCell* cell = node->data;
        if(cell->cell_x == x && cell->cell_y == y)
        {
            return cell;
        }
    }

    return NULL;
}

void map_get_screen_coord_by_world_coord(struct Map* map, int world_x, int world_y, int* screen_x, int* screen_y)
{
    int xstart = clamp(g_you->mon->x - (MCOLS/2), 0, (map->width * g_map_cell_width) - MCOLS - 1);
    int ystart = clamp(g_you->mon->y - (MROWS/2), 0, (map->height * g_map_cell_height) - MROWS - 1);

    *screen_x = world_x - xstart;
    *screen_y = world_y - ystart;
}

struct MapLocation* map_get_location(struct Map* map, int x, int y)
{
    struct MapCell* cell = map_get_cell_by_world_coord(map, x, y);
    if(!cell)
    {
        return NULL;
    }

    return map_cell_get_location(cell, x, y);
}

struct MapLocation* map_get_location_offset_by_direction(struct Map* map, struct MapLocation* loc, int input_keycode)
{
    int x_off = 0;
    int y_off = 0;

    switch((enum KeyCode)input_keycode)
    {
        case GAMEPLAY_COMMAND_MOVE_LEFT:
        {
            x_off = -1;
            break;
        }
        case GAMEPLAY_COMMAND_MOVE_RIGHT:
        {
            x_off = 1;
            break;
        }
        case GAMEPLAY_COMMAND_MOVE_UP:
        {
            y_off = -1;
            break;
        }
        case GAMEPLAY_COMMAND_MOVE_DOWN:
        {
            y_off = 1;
            break;
        }
        case GAMEPLAY_COMMAND_MOVE_LEFT_UP:
        {
            x_off = -1;
            y_off = -1;
            break;
        }
        case GAMEPLAY_COMMAND_MOVE_LEFT_DOWN:
        {
            x_off = -1;
            y_off = 1;
            break;
        }
        case GAMEPLAY_COMMAND_MOVE_RIGHT_UP:
        {
            x_off = 1;
            y_off = -1;
            break;
        }
        case GAMEPLAY_COMMAND_MOVE_RIGHT_DOWN:
        {
            x_off = 1;
            y_off = 1;
            break;
        }
        default:
        {
            break;
        }
    }

    const int newx = loc->x + x_off;
    const int newy = loc->y + y_off;

    //struct MapCell* cell = map_get_cell_by_world_coord(map, newx, newy);
    //if(!cell)
    //{
    //    return NULL;
    //}

    return map_get_location(map, newx, newy);
}

bool map_add_mon(struct Map* map, struct Mon* mon, int x, int y)
{
    struct MapCell* cell = map_get_cell_by_world_coord(map, x, y);
    if(!cell)
    {
        return false;
    }

    map_cell_add_mon(cell, mon);

    return true;
}

bool map_has_mon(struct Map* map, int x, int y)
{
    struct MapLocation* loc = map_get_location(map, x, y);

    return loc->mon != NULL;
}

bool map_is_in_bounds(struct Map* map, int x, int y)
{
    struct MapCell* cell = map_get_cell_by_world_coord(map, x, y);

    return cell != NULL;
}

// TODO: Replace with proper map view functionality
bool map_is_in_view_bounds(struct Map* map, int x, int y)
{
    int xmin = clamp(g_you->mon->x - (MCOLS/2), 0, (map->width * g_map_cell_width) - MCOLS - 1);
    int ymin = clamp(g_you->mon->y - (MROWS/2), 0, (map->height * g_map_cell_height) - MROWS - 1);

    int xmax = clamp(g_you->mon->x + (MCOLS/2), 0, (map->width * g_map_cell_width) + MCOLS - 1);
    int ymax = clamp(g_you->mon->y + (MROWS/2), 0, (map->height * g_map_cell_height) + MROWS - 1);

    return (x >= xmin && x < xmax) && (y >= ymin && y < ymax);
}

/**
 * Draw map
 */
void display_map(void)
{
    int xstart = clamp(g_you->mon->x - (MCOLS/2), 0, (g_cmap->width * g_map_cell_width) - MCOLS - 1);
    int ystart = clamp(g_you->mon->y - (MROWS/2), 0, (g_cmap->height * g_map_cell_height) - MROWS - 1);

    int i = 0;
    for(int x = xstart; i < MCOLS; ++x, ++i)
    {
        int j = 0;
        for(int y = ystart; j < MROWS; ++y, ++j)
        {
            struct MapLocation* loc = map_get_location(g_cmap, x, y);

            if(!mon_can_see(g_you->mon, x, y))
            {
                if(loc->seen)
                {
                    if(loc->feature)
                    {
                        term_draw_symbol(i, j, COL(CLR_FOG_OF_WAR), COL(CLR_DEFAULT), 0, loc->feature->symbol->sym);
                    }
                    else
                    {
                        term_draw_symbol(i, j, COL(CLR_FOG_OF_WAR), COL(CLR_DEFAULT), 0, loc->symbol.sym);
                    }
                }
                else
                {
                    term_draw_symbol(i, j, COL(CLR_DEFAULT), COL(CLR_DEFAULT), 0, ' ');
                }
            }
            else
            {
                loc->seen = true;

                if(loc->mon)
                {
                    // Draw mon
                    term_draw_symbol(i, j, &loc->mon->type->symbol->fg, &loc->mon->type->symbol->bg, loc->mon->type->symbol->attr, loc->mon->type->symbol->sym);
                }
                else if(loc_has_obj(loc))
                {
                    // Draw object
                    struct Object* obj = loc_get_obj(loc);
                    term_draw_symbol(i, j, &obj->symbol->fg, &obj->symbol->bg, obj->symbol->attr, obj->symbol->sym);
                }
                else if(loc->feature)
                {
                    // Draw feature
                    term_draw_symbol(i, j, &loc->feature->symbol->fg, &loc->feature->symbol->bg, loc->feature->symbol->attr, loc->feature->symbol->sym);
                }
                else
                {
                    // Draw base floor
                    term_draw_symbol(i, j, &loc->symbol.fg, &loc->symbol.bg, loc->symbol.attr, loc->symbol.sym);
                }
            }
        }
    }
}
