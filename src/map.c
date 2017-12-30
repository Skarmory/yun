#include "map.h"
#include <stdlib.h>
#include <curses.h>
#include "util.h"

#define MROWS 40
#define MCOLS 80

struct Map* cmap;

/* Mallocs map and sets map locations to default values */
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

/* Draws a map by drawing square rooms at random locations, and with random dimensions. */
void gen_rooms(void)
{
    int attempts = 200;
    cmap->rooms = (struct Room**)malloc(sizeof(struct Room*) * attempts);

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
            struct Room* tmp = cmap->rooms[r];

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

            cmap->rooms[rcount] = (struct Room*)malloc(sizeof(struct Room));
            struct Room* room = cmap->rooms[rcount];

            room->x = x;
            room->y = y;
            room->w = w;
            room->h = h;

            // Draw vertical walls
            for(int tmp = 0; tmp < h; tmp++)
            {
                cmap->locs[x][y+tmp].terrain = '|';
                cmap->locs[x][y+tmp].pathing = 0;
                cmap->locs[x+w-1][y+tmp].terrain = '|';
                cmap->locs[x+w-1][y+tmp].pathing = 0;
            }

            // Draw horizontal walls
            for(int tmp = 0; tmp < w; tmp++)
            {
                cmap->locs[x+tmp][y].terrain = '-';
                cmap->locs[x+tmp][y].pathing = 0;
                cmap->locs[x+tmp][y+h-1].terrain = '-';
                cmap->locs[x+tmp][y+h-1].pathing = 0;
            }

            // Fill in with floor
            for(int tmpx = 1; tmpx < w-1; tmpx++)
            for(int tmpy = 1; tmpy < h-1; tmpy++)
            {
                cmap->locs[x + tmpx][y + tmpy].terrain = '.';
                cmap->locs[x + tmpx][y + tmpy].pathing |= WALKABLE;
            }
        }
    }

    cmap->rooms = realloc(cmap->rooms, sizeof(struct Room*) * (rcount+1));
    cmap->room_count = rcount + 1;
}

/* Check if a location is a valid starting node for a maze
 * Valid maze starting nodes are solid rock surrounded by 8 other solid rock */
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

/* Finds a maze starting node */
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

/* Checks to see if a location is a viable maze node
 * Valid maze nodes need to have only 1 connection to another corridor tile.
 * They can also only have 2 of 4 diagonal squares as corridor tiles so long as those tiles
 * are orthogonal to the single connecting corridor tile. */
bool _is_valid_maze_node(struct Location* loc)
{
    if(loc->terrain != ' ')
        return false;

    if(loc->x == 0 || loc->y == 0 || loc->x == MCOLS-1 || loc->y == MROWS-1)
        return false;

    int conn_count = 0;

    struct Location* conn;

    if(cmap->locs[loc->x-1][loc->y].terrain == '#')
    {
        conn_count++;
        conn = &cmap->locs[loc->x-1][loc->y];
    }

    if(cmap->locs[loc->x+1][loc->y].terrain == '#')
    {
        conn_count++;
        conn = &cmap->locs[loc->x+1][loc->y];
    }

    if(cmap->locs[loc->x][loc->y-1].terrain == '#')
    {
        conn_count++;
        conn = &cmap->locs[loc->x][loc->y-1];
    }

    if(cmap->locs[loc->x][loc->y+1].terrain == '#')
    {
        conn_count++;
        conn = &cmap->locs[loc->x][loc->y+1];
    }

    if(conn_count > 1)
        return false;

    int xoff = loc->x - conn->x;
    int yoff = loc->y - conn->y;

    if(xoff == -1)
    {
        if(cmap->locs[loc->x-1][loc->y-1].terrain == '#' ||
           cmap->locs[loc->x-1][loc->y+1].terrain == '#')
            return false;
    }
    else if(xoff == 1)
    {
        if(cmap->locs[loc->x+1][loc->y-1].terrain == '#' ||
           cmap->locs[loc->x+1][loc->y+1].terrain == '#')
            return false;
    }
    else if(yoff == 1)
    {
        if(cmap->locs[loc->x-1][loc->y+1].terrain == '#' ||
           cmap->locs[loc->x+1][loc->y+1].terrain == '#')
            return false;
    }
    else
    {
        if(cmap->locs[loc->x-1][loc->y-1].terrain == '#' ||
           cmap->locs[loc->x+1][loc->y-1].terrain == '#')
            return false;
    }

   return true;
}

/* Attempts to get a location at random orthogonal to a maze node to continue the maze with.
 * It will fall back to just checking every single orthogonal direction if the randomly picked
 * direction is not valid. */
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

/* Recursive algorithm to carve out a maze in the level. */
void _flood_fill_maze(struct Location* loc)
{
    struct Location* next;

    while(_get_valid_maze_node(loc, &next))
    {
        next->terrain = '#';
        next->pathing |= WALKABLE;
        _flood_fill_maze(next);
    }
}

/* Checks if a given location is a deadend.
 * Deadends have only one connecting corridor tile */
bool _is_maze_deadend(struct Location* loc)
{
    int x = loc->x;
    int y = loc->y;

    if(loc->terrain != '#')
        return false;

    if(x == 0 || y == 0 || x == MCOLS-1 || y == MROWS-1)
        return false;

    int conn_count = 0;

    if(cmap->locs[x-1][y].terrain == '#' || cmap->locs[x-1][y].terrain == '.')
        conn_count++;

    if(cmap->locs[x+1][y].terrain == '#' || cmap->locs[x+1][y].terrain == '.')
        conn_count++;

    if(cmap->locs[x][y+1].terrain == '#' || cmap->locs[x][y+1].terrain == '.')
        conn_count++;

    if(cmap->locs[x][y-1].terrain == '#' || cmap->locs[x][y-1].terrain == '.')
        conn_count++;

    if(conn_count > 1)
        return false;

    return true;
}

/* Finds and sets the input arg to the first deadend it finds.
 * Returns true if it finds a deadend. */
bool _get_maze_deadend(struct Location** loc)
{
    for(int x = 0; x < MCOLS; x++)
    for(int y = 0; y < MROWS; y++)
    {
        if(_is_maze_deadend(&cmap->locs[x][y]))
        {
            *loc = &cmap->locs[x][y];
            return true;
        }
    }

    return false;
}

/* Finds the next deadend node next to the given one, and sets the input arg to it.
 * Returns true if it finds a deadend */
bool _get_next_deadend_node(struct Location* loc, struct Location** next)
{
    int x = loc->x;
    int y = loc->y;

    *next = &cmap->locs[x-1][y];
    if(_is_maze_deadend(*next))
        return true;

    *next = &cmap->locs[x+1][y];
    if(_is_maze_deadend(*next))
        return true;

    *next = &cmap->locs[x][y-1];
    if(_is_maze_deadend(*next))
        return true;

    *next = &cmap->locs[x][y+1];
    if(_is_maze_deadend(*next))
        return true;

    return false;
}

/* Recursively fills in deadend corridors until only connected corridors remain. */
void _back_fill_deadends(struct Location* loc)
{
    struct Location* next;
    while(_get_next_deadend_node(loc, &next))
    {
        next->terrain = ' ';
        next->pathing = 0;
        _back_fill_deadends(next);
    }
}

/* Picks wall sections on each wall at random until it finds one that has a corridor next to it.
 * It converts that wall section into an entryway into the room.
 * TODO: Generate a door object in the entryway tile */
void _make_doors(void)
{
    struct Room* room;

    struct Location** connectors = (struct Location**) malloc(sizeof(struct Location*) * (10 * 2 + 8 * 2));

    for(int i = 0; i < cmap->room_count; i++)
    {
        room = cmap->rooms[i];

        int cidx = 0;
        int x, y;

        // North wall
        x = room->x+1;
        y = room->y;
        if(y-1 > 0)
        {
            for(; x < (room->x + room->w - 1); x++)
            {
                if(cmap->locs[x][y-1].terrain == '#')
                {
                    connectors[cidx] = &cmap->locs[x][y];
                    cidx++;
                }
            }
        }

        // South wall
        x = room->x+1;
        y = room->y + room->h-1;
        if(y+1 < MROWS-1)
        {
            for(; x < (room->x + room->w - 1); x++)
            {
                if(cmap->locs[x][y+1].terrain == '#')
                {
                    connectors[cidx] = &cmap->locs[x][y];
                    cidx++;
                }
            }
        }

        // West wall
        x = room->x;
        y = room->y+1;
        if(x-1 > 0)
        {
            for(; y < (room->y + room->h - 1); y++)
            {
                if(cmap->locs[x-1][y].terrain == '#')
                {
                    connectors[cidx] = &cmap->locs[x][y];
                    cidx++;
                }
            }
        }

        // East wall
        x = room->x + room->w - 1;
        y = room->y+1;
        if(x+1 < MCOLS-1)
        {
            for(; y < (room->y + room->h - 1); y++)
            {
                if(cmap->locs[x+1][y].terrain == '#')
                {
                    connectors[cidx] = &cmap->locs[x][y];
                    cidx++;
                }
            }
        }

        int which = random_int(0, cidx-1);

        connectors[which]->terrain = '.';
        connectors[which]->pathing |= WALKABLE;
    }

    free(connectors);
}

/* Generate the maze of corridors, adds entryways into rooms, and fills in the deadends */
void gen_maze(void)
{
    struct Location* tmp;

    while(_get_maze_snode(&tmp))
    {
        tmp->terrain = '#';
        tmp->pathing |= WALKABLE;
        _flood_fill_maze(tmp);
    }

    _make_doors();

    while(_get_maze_deadend(&tmp))
    {
        tmp->terrain = ' ';
        tmp->pathing = 0;
        _back_fill_deadends(tmp);
    }
}

/* Call this to generate a map that contains rooms and a load of connecting corridors */
void gen_map(void)
{
    gen_rooms();
    gen_maze();
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

/* Do map bounds checking */
bool _valid_move(int x, int y)
{
    if(x < 0 || x >= MCOLS || y < 0 || y >= MROWS)
        return false;

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
