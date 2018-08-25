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
bool _valid_map_loc(int x, int y)
{
    if(x < 0 || x >= MCOLS || y < 0 || y >= MROWS)
        return false;
    return true;
}

    if(!(cmap->locs[x][y].pathing & WALKABLE))
        return false;

    if(cmap->locs[x][y].mon != NULL)
        return false;

    return true;
}

/* Change monster location */
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
