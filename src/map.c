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

void subdivide_map(int level, const int min, int x, int y, int w, int h)
{
    if(level == min)
        return;

    int opt = rand() % 2;
    if(opt == 0 && w >= 6)
    {
        // vertical
        int _x = x + w/2;

        for(int _y = 0; _y < (y + h); _y++)
        {
            cmap->locs[_x][_y].terrain = '|';
        }

       subdivide_map(level+1, min, x, y, w/2, h);
       subdivide_map(level+1, min, _x, y, w/2, h); 
    }
    else if(opt == 1 && h >= 6)
    {
        // horizontal
        int _y = y + h/2;
        
        for(int _x = 0; _x < (x + w); _x++)
        {
            cmap->locs[_x][_y].terrain = '=';
        } 

        subdivide_map(level+1, min, x, y, w, h/2);
        subdivide_map(level+1, min, x, _y, w, h/2); 
    }
}

/* Draws a random map by drawing squares randomly. Squares start off large, and few, but finish small and many
 * When a square is drawn, it fills in the insides of its walls with dungeon floor, this reduces a lot of the clutter caused by wall overlap
 */
void gen_by_squares(void)
{
    write_debug_msg("Begin gen_by_squares");

    int attempts = 200;
    struct Room* rooms = malloc(sizeof(struct Room) * attempts);

    int rcount = -1;
    for(int i = 0; i < attempts; i++)
    {
        int w = random_int(4, 10);
        int h = random_int(4, 10);

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

void gen_map(void)
{
    gen_by_squares();
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
