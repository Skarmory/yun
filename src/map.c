#include "map.h"
#include <stdlib.h>
#include <curses.h>

#define MROWS 40
#define MCOLS 80

struct Map* cmap;

void init_map(void)
{
    cmap = (struct Map*) malloc(sizeof(struct Map));
    cmap->glyphs = (char**) malloc(sizeof(char*) * MCOLS);
    cmap->mons = (struct Mon**) malloc(sizeof(struct Mon*) * MCOLS);
    cmap->monlist = (struct Mon*) malloc(sizeof(struct Mon) * 100);

    for(int i = 0; i < MCOLS; ++i)
    {
        cmap->glyphs[i] = (char*) malloc(sizeof(char) * MROWS);
        cmap->mons[i] = (struct Mon*) malloc(sizeof(struct Mon) * MROWS);

        for(int j = 0; j < MROWS; ++j)
        {
            cmap->glyphs[i][j] = '.';
            cmap->mons[i][j] = MON_NULL;
        }
    }

}

void display_map(void)
{
    clear();

    for(int i = 0; i < MCOLS; ++i)
    for(int j = 0; j < MROWS; ++j)
    {
        if(cmap->mons[i][j] != MON_NULL)
            mvaddch(j, i, cmap->mons[i][j]->type->sym);
        else
            mvaddch(j, i, cmap->glyphs[i][j]);
    }
}

void add_mon(struct Mon* mon)
{
    int x = mon->x;
    int y = mon->y;
    cmap->mons[x][y] = mon;

    cmap->monlist
}

void rm_mon(struct Mon* mon)
{
    for(struct Mon* tmp = cmap->monlist; tmp; tmp = tmp->next)
    {
        if(mon == tmp)
        {
        }
    }
}
