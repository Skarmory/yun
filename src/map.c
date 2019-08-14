#include "map.h"

#include "colour.h"
#include "list.h"
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

struct Map* cmap = NULL;

/**
 * Creates the map and sets map locations to default values
 */
struct Map* map_new(int width, int height)
{
    struct Map* map = (struct Map*) malloc(sizeof(struct Map));
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
    ListNode *node, *next;
    list_for_each_safe(&map->cell_list, node, next)
    {
        map_cell_free(node->data);
        free(node);
    }

    free(map);
}

struct MapCell* map_get_cell_by_world_coord(struct Map* map, int x, int y)
{
    ListNode* node;
    list_for_each(&map->cell_list, node)
    {
        if(map_cell_is_in_bounds(node->data, x, y))
            return node->data;
    }

    return NULL;
}

struct MapCell* map_get_cell_by_map_coord(struct Map* map, int x, int y)
{
    ListNode* node;
    list_for_each(&map->cell_list, node)
    {
        struct MapCell* cell = node->data;
        if(cell->cell_x == x && cell->cell_y == y)
            return cell;
    }

    return NULL;
}

/**
 * Draw map
 */
void display_map(void)
{
    int xstart = clamp(you->mon->x - (MCOLS/2), 0, (cmap->width * g_map_cell_width) - MCOLS - 1);
    int ystart = clamp(you->mon->y - (MROWS/2), 0, (cmap->height * g_map_cell_height) - MROWS - 1);

    int i = 0;
    for(int x = xstart; i < MCOLS; ++x, ++i)
    {
        int j = 0;
        for(int y = ystart; j < MROWS; ++y, ++j)
        {
            struct MapCell* cell = map_get_cell_by_world_coord(cmap, x, y);
            struct MapLocation* loc = map_cell_get_location(cell, x, y);

            if(!mon_can_see(you->mon, x, y))
            {
                if(loc->seen)
                    term_draw_symbol(i, j, COL(CLR_FOG_OF_WAR), COL(CLR_DEFAULT), 0, loc->symbol.sym);
                else
                    term_draw_symbol(i, j, COL(CLR_DEFAULT), COL(CLR_DEFAULT), 0, ' ');
            }
            else
            {
                struct Object* obj = loc->obj_list.head ? loc->obj_list.head->data : NULL;

                loc->seen = true;

                if(loc->mon)
                {
                    term_draw_symbol(i, j, &loc->mon->type->symbol->fg, &loc->mon->type->symbol->bg, loc->mon->type->symbol->attr, loc->mon->type->symbol->sym);
                }
                else if(obj)
                {
                    term_draw_symbol(i, j, &obj->symbol->fg, &obj->symbol->bg, obj->symbol->attr, obj->symbol->sym);
                }
                else
                {
                    term_draw_symbol(i, j, &loc->symbol.fg, &loc->symbol.bg, loc->symbol.attr, loc->symbol.sym);
                }
            }
        }
    }

    term_refresh();
}
