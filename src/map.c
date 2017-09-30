#include "map.h"
#include <stdlib.h>
#include <curses.h>

#define MROWS 40
#define MCOLS 80

char** map;

void init_map(void)
{
    map = (char**) malloc(sizeof(char*) * MCOLS); 
    for(int i = 0; i < MCOLS; ++i)
    {
        map[i] = (char*) malloc(sizeof(char) * MROWS);
        for(int j = 0; j < MROWS; ++j)
        {
            map[i][j] = '.';
        }
    }
}

void display_map(void)
{
    clear();

    for(int i = 0; i < MCOLS; ++i)
    for(int j = 0; j < MROWS; ++j)
    {
        mvaddch(j, i, map[i][j]);
    }
}
