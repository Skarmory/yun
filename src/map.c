#include "map.h"
#include <stdlib.h>
#include <curses.h>
#include "util.h"

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
            cmap->locs[i][j].x = i;
            cmap->locs[i][j].y = j;
            cmap->locs[i][j].terrain = ' ';
            cmap->locs[i][j].mon = NULL;
        }
    }
}

/* Draws a random map by drawing squares randomly. Squares start off large, and few, but finish small and many
 * When a square is drawn, it fills in the insides of its walls with dungeon floor, this reduces a lot of the clutter caused by wall overlap
 */
void gen_rooms(void)
{
    int attempts = 2000;
    struct Room* rooms = malloc(sizeof(struct Room) * attempts);

    int rcount = -1;
    for(int i = 0; i < attempts; i++)
    {
        int w = random_int(4, 10);
        int h = random_int(4, 8);

        int x = random_int(0, (MCOLS - w - 1));
        int y = random_int(0, (MROWS - h - 1));

        bool gen = true;

        for(int r = 0; r <= rcount; r++)
        {
            struct Room* tmp = &rooms[r];

            if(
                (((x >= tmp->x) && (x <= (tmp->x + tmp->w))) ||
                 ((tmp->x >= x) && (tmp->x <= (x + w)))) &&
                (((y >= tmp->y) && (y <= (tmp->y + tmp->h))) ||
                 ((tmp->y >= y) && (tmp->y <= (y + h))))
            )
            {
                gen = false;
                break;
            }
        }

        if(gen)
        {
            rcount++;
            struct Room* room = &rooms[rcount];
            room->x = x;
            room->y = y;
            room->w = w;
            room->h = h;

            // Draw vertical walls
            for(int tmp = 0; tmp < h; tmp++)
            {
                cmap->locs[x][y+tmp].terrain = '|';
                cmap->locs[x+w-1][y+tmp].terrain = '|';
            }

            // Draw horizontal walls
            for(int tmp = 0; tmp < w; tmp++)
            {
                cmap->locs[x+tmp][y].terrain = '=';
                cmap->locs[x+tmp][y+h].terrain = '=';
            }

            // Fill in with floor
            for(int tmpx = 1; tmpx < w-1; tmpx++)
            for(int tmpy = 1; tmpy < h; tmpy++)
            {
                cmap->locs[x + tmpx][y + tmpy].terrain = '.';
            }
        }
    }

    free(rooms);
}

bool _is_maze_snode(struct Location* loc)
{
    int x = loc->x;
    int y = loc->y;

    if(x == 0 || x == MCOLS - 1 || y == 0 || y == MROWS - 1 || cmap->locs[x][y].terrain != ' ')
        return false;

    if(cmap->locs[x-1][y-1].terrain == ' ' && cmap->locs[x][y-1].terrain == ' ' && cmap->locs[x+1][y-1].terrain == ' ' &&
       cmap->locs[x-1][y].terrain   == ' ' && cmap->locs[x+1][y].terrain == ' ' &&
       cmap->locs[x-1][y+1].terrain == ' ' && cmap->locs[x][y+1].terrain == ' ' && cmap->locs[x+1][y+1].terrain == ' ')
        return true;

    return false;
}

bool _get_maze_snode(struct Location** tmp)
{
    for(int x = 0; x < MCOLS; x++)
    for(int y = 0; y < MROWS; y++)
    {
        if(_is_maze_snode(&cmap->locs[x][y]))
        {
            *tmp = &(cmap->locs[x][y]);
            return true;
        }
    }

    return false;
}

bool _is_valid_maze_node(struct Location* loc)
{
    if(loc->terrain != ' ')
        return false;

    if(loc->x == 0 || loc->y == 0 || loc->x == MCOLS-1 || loc->y == MROWS-1)
        return false;

    int conn_count = 0;

    if(cmap->locs[loc->x-1][loc->y].terrain == '#')
        conn_count++;

    if(cmap->locs[loc->x+1][loc->y].terrain == '#')
        conn_count++;

    if(cmap->locs[loc->x][loc->y-1].terrain == '#')
        conn_count++;

    if(cmap->locs[loc->x][loc->y+1].terrain == '#')
        conn_count++;

    if(conn_count > 1)
        return false;

    return true;

}

bool _get_valid_maze_node(struct Location* loc, struct Location** next)
{
    int x = loc->x;
    int y = loc->y;

    int xoff = 0;
    int yoff = 0;

    int dir = random_int(0, 3);

    switch(dir)
    {
        case 0:
            xoff = -1; break;
        case 1:
            xoff = 1; break;
        case 2:
            yoff = -1; break;
        case 3:
            yoff= 1; break;
    }

    *next = &cmap->locs[x + xoff][y + yoff];
    if(_is_valid_maze_node(*next))
        return true;

    *next = &cmap->locs[x+1][y];
    if(_is_valid_maze_node(*next))
        return true;

    *next = &cmap->locs[x-1][y];
    if(_is_valid_maze_node(*next))
        return true;

    *next = &cmap->locs[x][y+1];
    if(_is_valid_maze_node(*next))
        return true;

    *next = &cmap->locs[x][y-1];
    if(_is_valid_maze_node(*next))
        return true;

    return false;
}

void fill(struct Location* loc)
{
    struct Location* next;

    while(_get_valid_maze_node(loc, &next))
    {
        next->terrain = '#';
        fill(next);
    }
}

void gen_maze(void)
{
    struct Location* tmp;

    while(_get_maze_snode(&tmp))
    {
        tmp->terrain = '#';
        fill(tmp);
    }
}

void gen_map(void)
{
    gen_rooms();
    gen_maze();
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
