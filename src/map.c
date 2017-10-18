#include "map.h"
#include <stdlib.h>
#include <curses.h>

#define MROWS 40
#define MCOLS 80

struct Map* cmap;

// Initialise new map
void init_map(void)
{
    cmap = (struct Map*) malloc(sizeof(struct Map));
    cmap->locs = (struct Location**) malloc(sizeof(struct Location*) * MCOLS);
    cmap->monlist = (struct Mon*) malloc(sizeof(struct Mon));
    cmap->monlist = NULL;

    for(int i = 0; i < MCOLS; ++i)
    {
        cmap->locs[i] = (struct Location*) malloc(sizeof(struct Location) * MROWS);

        for(int j = 0; j < MROWS; ++j)
        {
            cmap->locs[i][j].terrain = '.';
            cmap->locs[i][j].mon = NULL;
        }
    }
}

// Draw map using ncurses
void display_map(void)
{
    for(int i = 0; i < MCOLS; ++i)
    for(int j = 0; j < MROWS; ++j)
    {
        struct Location* loc = &cmap->locs[i][j];

        if(loc->mon != NULL)

            mvaddch(j, i, loc->mon->type->sym);
        else
            mvaddch(j, i, loc->terrain);
    }

    refresh();
}

// Add monster to the level
void add_mon(struct Mon* mon)
{
    int x = mon->x;
    int y = mon->y;
    cmap->locs[x][y].mon = mon;

    // push mon onto the list
    mon->next = cmap->monlist;
    cmap->monlist = mon->next;
}

// Remove monster from the level
void rm_mon(struct Mon* mon)
{
    struct Mon* tmp = cmap->monlist;
    struct Mon* pre = cmap->monlist;

    while(tmp != NULL)
    {
        if(tmp == mon)
        {
            cmap->locs[mon->x][mon->y].mon = NULL;
            pre->next = tmp->next;
            tmp->next = NULL;
        }

        pre = tmp;
        tmp = tmp->next;
    }
}

// Do map bounds checking
bool _valid_move(int x, int y)
{
    return (x >= 0 && x < MCOLS) && (y >= 0 && y < MROWS);
}

// Change monster location
bool move_mon(struct Mon* mon, int newx, int newy)
{
    if(!_valid_move(newx, newy))
        return false;

    cmap->locs[mon->x][mon->y].mon = NULL;
    cmap->locs[newx][newy].mon = mon;
    mon->x = newx;
    mon->y = newy;

    return true;
}
