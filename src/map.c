#include "map.h"

#include "list.h"
#include "log.h"
#include "mon.h"
#include "montype.h"
#include "ncurses_ext.h"
#include "object.h"
#include "player.h"
#include "path.h"
#include "symbol.h"
#include "util.h"

#include <ncurses.h>
#include <stdlib.h>

struct Map* cmap = NULL;

void _map_free_all_mons(struct Map* map)
{
    struct Mon* curr = map->monlist;
    struct Mon* next;
    while(curr)
    {
        next = list_next(curr, struct Mon, map_mons);
        list_rm(curr, map_mons);
        free_mon(curr);
        curr = next;
    }
}

/**
 * Creates the map and sets map locations to default values
 */
struct Map* new_map(void)
{
    struct Map* map = (struct Map*) malloc(sizeof(struct Map));
    map->locs = (struct Location**) malloc(sizeof(struct Location*) * MCOLS);
    map->monlist = NULL;
    map->rooms = NULL;
    map->room_count = 0;

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
            map->locs[i][j].objects = NULL;
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
            struct Object* curr = map->locs[x][y].objects;
            struct Object* next;

            while(curr)
            {
                next = list_next(curr, struct Object, obj_list);

                list_rm(curr, obj_list);
                free_obj(curr);

                curr = next;
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

        if(loc->mon != NULL)
            draw_symbol(i, j, loc->mon->type->symbol->sym, loc->mon->type->symbol->fg, loc->mon->type->symbol->attr);
        else if(loc->objects != NULL)
            draw_symbol(i, j, loc->objects->symbol->sym, loc->objects->symbol->fg, loc->objects->symbol->attr);
        else
            draw_symbol(i, j, loc->terrain, 0, 0);
    }

    draw_symbol(you->mon->x, you->mon->y, '@', you->mon->type->symbol->fg, you->mon->type->symbol->attr);

    refresh();
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
    list_add(mon, map->monlist, map_mons);
    map->monlist = mon;
}

/*
 * Remove monster from the level
 */
bool map_rm_mon(struct Map* map, struct Mon* mon)
{
    struct Mon* curr = map->monlist;
    while(curr)
    {
        if(curr == mon)
        {
            map->locs[mon->x][mon->y].mon = NULL;

            if(list_is_head(curr, map_mons))
                map->monlist = list_next(curr, struct Mon, map_mons);

            list_rm(mon, map_mons);
            return true;
        }

        curr = list_next(curr, struct Mon, map_mons);
    }

    return false;
}

/**
 * Get the object at given map location
 */
struct Object* map_get_objects(struct Map* map, int x, int y)
{
    return map->locs[x][y].objects;
}

/**
 * Add object to given map location
 */
bool loc_add_obj(struct Location* loc, struct Object* obj)
{
    // push object onto location object linked list
    list_add(obj, loc->objects, obj_list);
    loc->objects = obj;

    return true;
}

/**
 * Remove object from given map location
 */
bool loc_rm_obj(struct Location* loc, struct Object* obj)
{
    struct Object* curr = loc->objects;
    while(curr)
    {
        if(curr == obj)
        {
            if(list_is_head(curr, obj_list))
                loc->objects = list_next(curr, struct Object, obj_list);

            list_rm(curr, obj_list);
            return true;
        }

        curr = list_next(curr, struct Object, obj_list);
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
