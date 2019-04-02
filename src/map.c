#include "map.h"

#include "list.h"
#include "log.h"
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

void _map_free_all_mons(struct Map* map)
{
    ListNode *node, *n;
    list_for_each_safe(&map->mon_list, node, n)
    {
        free_mon(node->data);
        free(node);
    }
}

/**
 * Creates the map and sets map locations to default values
 */
struct Map* new_map(void)
{
    struct Map* map = (struct Map*) malloc(sizeof(struct Map));
    map->locs = (struct Location**) malloc(sizeof(struct Location*) * MCOLS);
    map->rooms = NULL;
    map->room_count = 0;
    list_init(&map->mon_list);

    for(int i = 0; i < MCOLS; ++i)
    {
        map->locs[i] = (struct Location*) malloc(sizeof(struct Location) * MROWS);

        for(int j = 0; j < MROWS; ++j)
        {
            map->locs[i][j].x = i;
            map->locs[i][j].y = j;
            map->locs[i][j].terrain = ' ';
            map->locs[i][j].path_node = new_path_node(&map->locs[i][j]);
            map->locs[i][j].mon = NULL;
            map->locs[i][j].pathing = 0;
            list_init(&map->locs[i][j].obj_list);
        }
    }

    return map;
}

/**
 * Safely deletes the map
 */
void free_map(struct Map* map)
{
    _map_free_all_mons(map);

    for(int r = 0; r < map->room_count; r++)
        free(map->rooms[r]);

    free(map->rooms);

    for(int x = 0; x < MCOLS; x++)
    {
        for(int y = 0; y < MROWS; y++)
        {
            ListNode *node, *n;
            list_for_each_safe(&map->locs[x][y].obj_list, node, n)
            {
                free_obj(node->data);
                free(node);
            }

            free(map->locs[x][y].path_node);
        }

        free(map->locs[x]);
    }

    free(map->locs);
    free(map);
}

/**
 * Draw map using ncurses
 */
void display_map(void)
{
    for(int i = 0; i < MCOLS; ++i)
    for(int j = 0; j < MROWS; ++j)
    {
        struct Location* loc = &cmap->locs[i][j];
        struct Object* obj = loc->obj_list.head ? loc->obj_list.head->data : NULL;

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
            term_draw_symbol(i, j, NULL, NULL, 0, loc->terrain);
        }
    }

    term_draw_symbol(you->mon->x, you->mon->y, &you->mon->type->symbol->fg, &you->mon->type->symbol->bg, you->mon->type->symbol->attr, '@');

    term_refresh();
}

/**
 * Add monster to the level
 */
void map_add_mon(struct Map* map, struct Mon* mon)
{
    int x = mon->x;
    int y = mon->y;
    map->locs[x][y].mon = mon;

    // push mon onto the list
    list_add(&map->mon_list, mon);
}

/*
 * Remove monster from the level
 */
bool map_rm_mon(struct Map* map, struct Mon* mon)
{
    ListNode* node = list_find(&map->mon_list, mon);
    if(node)
    {
        map->locs[mon->x][mon->y].mon = NULL;
        list_rm(&map->mon_list, node);
        return true;
    }

    return false;
}

/**
 * Get the object at given map location
 */
List* map_get_objects(struct Map* map, int x, int y)
{
    return &map->locs[x][y].obj_list;
}

/**
 * Add object to given map location
 */
bool loc_add_obj(struct Location* loc, struct Object* obj)
{
    // push object onto location object linked list
    list_add(&loc->obj_list, obj);

    return true;
}

/**
 * Remove object from given map location
 */
bool loc_rm_obj(struct Location* loc, struct Object* obj)
{
    ListNode* node = list_find(&loc->obj_list, obj);
    if(node)
    {
        list_rm(&loc->obj_list, node);
        return true;
    }
    return false;
}

/*
 * Does map boundary check
 */
bool map_in_bounds(struct Map* map, int x, int y)
{
    if(x < 0 || x >= MCOLS || y < 0 || y >= MROWS)
        return false;
    return true;
}

/**
 * Check if a location has a mon
 */
bool map_has_mon(struct Map* map, int x, int y)
{
    return map->locs[x][y].mon != NULL;
}

/**
 * Check if location is pathable given sample path bits
 */
bool map_is_pathable(struct Map* map, int x, int y, int path_bits)
{
    return map->locs[x][y].pathing & path_bits;
}

/**
 * Check if given x, y location is a valid move
 */
bool map_valid_move(struct Map* map, int x, int y, int path_bits)
{
    return map_in_bounds(map, x, y) && map_is_pathable(map, x, y, path_bits) && !map_has_mon(map, x, y);
}

/**
 * Change monster location
 */
bool map_move_mon(struct Map* map, struct Mon* mon, int newx, int newy)
{
    if(!map_valid_move(map, newx, newy, mon->pathing))
        return false;

    map->locs[mon->x][mon->y].mon = NULL;
    map->locs[newx][newy].mon = mon;
    mon->x = newx;
    mon->y = newy;

    return true;
}
