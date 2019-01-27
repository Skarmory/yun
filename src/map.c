#include "map.h"

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
    struct Mon* tmp;
    while((tmp = map->monlist) != NULL)
    {
        map->monlist = map->monlist->next;
        free_mon(tmp);
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
            struct Object* tmp;
            while((tmp = map->locs[x][y].objects) != NULL)
            {
                map->locs[x][y].objects = map->locs[x][y].objects->next;
                free_obj(tmp);
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
    mon->next = map->monlist;
    map->monlist = mon;
}

/*
 * Remove monster from the level
 */
bool map_rm_mon(struct Map* map, struct Mon* mon)
{
    struct Mon* curr = map->monlist;
    struct Mon* prev = NULL;
    while(curr)
    {
       if(curr == mon)
       {
            map->locs[mon->x][mon->y].mon = NULL;

            // Check if this is head of monlist
            if(prev)
                prev->next = curr->next;
            else
                map->monlist = curr->next;

            return true;
       }

       prev = curr;
       curr = curr->next;
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
    obj->next = loc->objects;
    loc->objects = obj;

    return true;
}

/**
 * Remove object from given map location
 */
bool loc_rm_obj(struct Location* loc, struct Object* obj)
{
    struct Object* curr = loc->objects;
    struct Object* prev = NULL;

    while(curr)
    {
       if(curr == obj)
       {
            // Check if this is head of monlist
            if(prev)
                prev->next = curr->next;
            else
                loc->objects = curr->next;

            return true;
       }

       prev = curr;
       curr = curr->next;
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

/*
 * Get an array of the neighbouring locations to given location
 */
int map_loc_get_neighbours(struct Map* map, struct Location* loc, struct Location*** locs)
{
    *locs = (struct Location**)malloc(sizeof(struct Location*) * 8);

    int x = loc->x;
    int y = loc->y;

    int count = 0;

    for(int _x = x-1; _x < x+2; _x++)
    for(int _y = y-1; _y < y+2; _y++)
    {
        if(_x == x && _y == y)
            continue;

        if(map_in_bounds(map, _x, _y))
        {
            (*locs)[count] = &map->locs[_x][_y];
            count++;
        }
    }

    if(count < 8)
        *locs = realloc(locs, count * sizeof(struct Location*));

    return count;
}
