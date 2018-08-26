#include "map.h"
#include "log.h"
#include <stdlib.h>
#include <curses.h>
#include "util.h"

struct Map* cmap;

/* Creates the map and sets map locations to default values */
void init_map(void)
{
    cmap = (struct Map*) malloc(sizeof(struct Map));
    cmap->locs = (struct Location**) malloc(sizeof(struct Location*) * MCOLS);
    cmap->monlist = NULL;

    for(int i = 0; i < MCOLS; ++i)
    {
        cmap->locs[i] = (struct Location*) malloc(sizeof(struct Location) * MROWS);

        for(int j = 0; j < MROWS; ++j)
        {
            cmap->locs[i][j].x = i;
            cmap->locs[i][j].y = j;
            cmap->locs[i][j].terrain = ' ';
            cmap->locs[i][j].mon = NULL;
            cmap->locs[i][j].pathing = 0;
        }
    }
}


/* Draw map using ncurses */
void display_map(void)
{
    for(int i = 0; i < MCOLS; ++i)
    for(int j = 0; j < MROWS; ++j)
    {
        struct Location* loc = &cmap->locs[i][j];

        if(loc->mon != NULL)
        {
            attron(loc->mon->type->attr);
            mvaddch(j, i, loc->mon->type->sym);
            attroff(loc->mon->type->attr);
        }
        else
            mvaddch(j, i, loc->terrain);
    }

    refresh();
}

/* Add monster to the level */
void add_mon(struct Mon* mon)
{
    int x = mon->x;
    int y = mon->y;
    cmap->locs[x][y].mon = mon;

    // push mon onto the list
    mon->next = cmap->monlist;
    cmap->monlist = mon;
}

/* Remove monster from the level */
bool rm_mon(struct Mon* mon)
{
    struct Mon* curr = cmap->monlist;
    struct Mon* prev = NULL;
    while(curr)
    {
       if(curr == mon)
       {
            cmap->locs[mon->x][mon->y].mon = NULL;

            // Check if this is head of monlist
            if(prev)
                prev->next = curr->next;
            else
                cmap->monlist = curr->next;

            return true;
       }

       prev = curr;
       curr = curr->next;
    }

    return false;
}

/* Does map boundary check */
bool loc_in_bounds(int x, int y)
{
    if(x < 0 || x >= MCOLS || y < 0 || y >= MROWS)
        return false;
    return true;
}

bool loc_has_mon(int x, int y)
{
    return cmap->locs[x][y].mon != NULL;
}

bool loc_is_pathable(int x, int y, int path_bits)
{
    return cmap->locs[x][y].pathing & path_bits;
}

/* Check if given x, y location is a valid move */
bool valid_move(int x, int y, int path_bits)
{
    return loc_in_bounds(x, y) && loc_is_pathable(x, y, path_bits) && !loc_has_mon(x, y);
}

/* Change monster location */
bool move_mon(struct Mon* mon, int newx, int newy)
{
    if(!valid_move(newx, newy, mon->pathing))
        return false;

    cmap->locs[mon->x][mon->y].mon = NULL;
    cmap->locs[newx][newy].mon = mon;
    mon->x = newx;
    mon->y = newy;

    return true;
}

/* Get an array of the neighbouring locations to given location */
int get_neighbours(struct Location* loc, struct Location*** locs)
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

        if(loc_in_bounds(_x, _y))
        {
            (*locs)[count] = &cmap->locs[_x][_y];
            count++;
        }
    }

    if(count < 8)
        *locs = realloc(locs, count * sizeof(struct Location*));

    return count;
}

/* Safely deletes the map */
void destroy_map(void)
{
    if(cmap != NULL)
    {
        for(int r = 0; r < cmap->room_count; r++)
            free(cmap->rooms[r]);

        free(cmap->rooms);

        for(int x = 0; x < MCOLS; x++)
            free(cmap->locs[x]);

        free(cmap->locs);
    }
}
