#include "map_gen.h"

#include "colour.h"
#include "log.h"
#include "map.h"
#include "map_cell.h"
#include "map_location.h"
#include "map_room.h"
#include "symbol.h"
#include "util.h"

#include <stdlib.h>

// TODO: Check over the maze gen algorithm for performance improvements
// TODO: Ensure that rooms are always connected

/* Draws a map by drawing square rooms at random locations, and with random dimensions. */
void gen_rooms(struct MapCell* cell)
{
    // Place as many rooms as we can, limited to 200 attempts
    int attempts = 200;
    ListNode* rnode = NULL;

    for(int i = 0; i < attempts; i++)
    {
        int w = random_int(4, 10);
        int h = random_int(4, 8);
        int x = random_int(0, g_map_cell_width-1-w);
        int y = random_int(0, g_map_cell_height-1-h);

        bool overlaps = false;

        list_for_each(&cell->room_list, rnode)
        {
            struct Room* r = rnode->data;
            if((((x >= r->x) && (x <= (r->x + r->w))) || ((r->x >= x) && (r->x <= (x + w)))) &&
               (((y >= r->y) && (y <= (r->y + r->h))) || ((r->y >= y) && (r->y <= (y + h)))))
            {
                overlaps = true;
                break;
            }
        }

        if(!overlaps)
        {
            struct Room* room = (struct Room*) malloc(sizeof(struct Room));
            room->x = x;
            room->y = y;
            room->w = w;
            room->h = h;

            list_add(&cell->room_list, room);
        }
    }

    // Draw all the rooms
    list_for_each(&cell->room_list, rnode)
    {
        struct Room* r = rnode->data;
        struct MapLocation* loc;

        // Draw vertical walls
        for(int tmp = 0; tmp < r->h; tmp++)
        {
            loc = map_cell_get_location(cell, r->x, r->y+tmp);
            loc->symbol.sym = '|';
            loc->symbol.bg = (struct Colour){20,20,20};
            loc->pathing_flags = 0;

            loc = map_cell_get_location(cell, r->x+r->w-1, r->y+tmp);
            loc->symbol.sym = '|';
            loc->symbol.bg = (struct Colour){20,20,20};
            loc->pathing_flags = 0;
        }

        // Draw horizontal walls
        for(int tmp = 0; tmp < r->w; tmp++)
        {
            loc = map_cell_get_location(cell, r->x+tmp, r->y);
            loc->symbol.sym = '-';
            loc->symbol.bg = (struct Colour){20,20,20};
            loc->pathing_flags = 0;

            loc = map_cell_get_location(cell, r->x+tmp, r->y+r->h-1);
            loc->symbol.sym = '-';
            loc->symbol.bg = (struct Colour){20,20,20};
            loc->pathing_flags = 0;
        }

        // Fill in with floor
        for(int tmpx = 1; tmpx < r->w-1; tmpx++)
        for(int tmpy = 1; tmpy < r->h-1; tmpy++)
        {
            int floor_col = random_int(20,180);
            loc = map_cell_get_location(cell, r->x+tmpx, r->y+tmpy);
            loc->symbol.sym = '.';
            loc->symbol.fg = (struct Colour){floor_col,floor_col,floor_col};
            loc->symbol.bg = ((tmpx+tmpy) % 2 == 0) ? (struct Colour){20,20,20} : (struct Colour){26,26,26};
            loc->pathing_flags |= PATHING_GROUND;
        }
    }
}

/* Check if a location is a valid starting node for a maze
 * Valid maze starting nodes are solid rock surrounded by 8 other solid rock */
bool _is_maze_snode(struct MapCell* cell, struct MapLocation* loc)
{
    int x = loc->x;
    int y = loc->y;

    if(x == 0 || x == g_map_cell_width - 1 || y == 0 || y == g_map_cell_height - 1 || loc->symbol.sym != ' ')
        return false;

    if(map_cell_get_location(cell, x-1, y-1)->symbol.sym == ' ' && map_cell_get_location(cell, x, y-1)->symbol.sym == ' ' && map_cell_get_location(cell, x+1, y-1)->symbol.sym == ' ' &&
       map_cell_get_location(cell, x-1, y)->symbol.sym   == ' ' && map_cell_get_location(cell, x+1, y)->symbol.sym == ' ' &&
       map_cell_get_location(cell, x-1, y+1)->symbol.sym == ' ' && map_cell_get_location(cell, x, y+1)->symbol.sym == ' ' && map_cell_get_location(cell, x+1, y+1)->symbol.sym == ' ')
        return true;

    return false;
}

/* Finds a maze starting node */
struct MapLocation* _get_maze_snode(struct MapCell* cell)
{
    for(int x = 0; x < g_map_cell_width; x++)
    for(int y = 0; y < g_map_cell_height; y++)
    {
        struct MapLocation* loc = map_cell_get_location(cell, x, y);
        if(_is_maze_snode(cell, loc))
        {
            return loc;
        }
    }

    return NULL;
}

/* Checks to see if a location is a viable maze node
 * Valid maze nodes need to have only 1 connection to another corridor tile.
 * They can also only have 2 of 4 diagonal squares as corridor tiles so long as those tiles
 * are orthogonal to the single connecting corridor tile.
 */
bool _is_valid_maze_node(struct MapCell* cell, struct MapLocation* loc)
{
    if(loc->symbol.sym != ' ')
        return false;

    if(loc->x == 0 || loc->y == 0 || loc->x == g_map_cell_width-1 || loc->y == g_map_cell_height-1)
        return false;

    int conn_count = 0;

    struct MapLocation* tmp;
    struct MapLocation* conn;

    tmp = map_cell_get_location(cell, loc->x-1, loc->y);
    if(tmp->symbol.sym == '#')
    {
        conn_count++;
        conn = tmp;
    }

    tmp = map_cell_get_location(cell, loc->x+1, loc->y);
    if(tmp->symbol.sym == '#')
    {
        conn_count++;
        conn = tmp;
    }

    tmp = map_cell_get_location(cell, loc->x, loc->y-1);
    if(tmp->symbol.sym == '#')
    {
        conn_count++;
        conn = tmp;
    }

    tmp = map_cell_get_location(cell, loc->x, loc->y+1);
    if(tmp->symbol.sym == '#')
    {
        conn_count++;
        conn = tmp;
    }

    if(conn_count > 1)
        return false;

    int xoff = loc->x - conn->x;
    int yoff = loc->y - conn->y;

    if(xoff == -1)
    {
        if(map_cell_get_location(cell, loc->x-1, loc->y-1)->symbol.sym == '#' ||
           map_cell_get_location(cell, loc->x-1, loc->y+1)->symbol.sym == '#')
            return false;
    }
    else if(xoff == 1)
    {
        if(map_cell_get_location(cell, loc->x+1, loc->y-1)->symbol.sym == '#' ||
           map_cell_get_location(cell, loc->x+1, loc->y+1)->symbol.sym == '#')
            return false;
    }
    else if(yoff == 1)
    {
        if(map_cell_get_location(cell, loc->x-1, loc->y+1)->symbol.sym == '#' ||
           map_cell_get_location(cell, loc->x+1, loc->y+1)->symbol.sym == '#')
            return false;
    }
    else
    {
        if(map_cell_get_location(cell, loc->x-1, loc->y-1)->symbol.sym == '#' ||
           map_cell_get_location(cell, loc->x+1, loc->y-1)->symbol.sym == '#')
            return false;
    }

   return true;
}

/* Attempts to get a location at random orthogonal to a maze node to continue the maze with.
 * It will fall back to just checking every single orthogonal direction if the randomly picked
 * direction is not valid. */
struct MapLocation* _get_valid_maze_node(struct MapCell* cell, struct MapLocation* loc)
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

    struct MapLocation* next;
    next = map_cell_get_location(cell, x+xoff, y+yoff);
    if(_is_valid_maze_node(cell, next))
        return next;

    next = map_cell_get_location(cell, x+1, y);
    if(_is_valid_maze_node(cell, next))
        return next;

    next = map_cell_get_location(cell, x-1, y);
    if(_is_valid_maze_node(cell, next))
        return next;

    next = map_cell_get_location(cell, x, y+1);
    if(_is_valid_maze_node(cell, next))
        return next;

    next = map_cell_get_location(cell, x, y-1);
    if(_is_valid_maze_node(cell, next))
        return next;

    return NULL;
}

/* Recursive algorithm to carve out a maze in the level. */
void _flood_fill_maze(struct MapCell* cell, struct MapLocation* loc)
{
    struct MapLocation* next;

    while((next = _get_valid_maze_node(cell, loc)))
    {
        next->symbol.sym = '#';
        next->pathing_flags |= PATHING_GROUND;
        _flood_fill_maze(cell, next);
    }
}

/* Checks if a given location is a deadend.
 * Deadends have only one connecting corridor tile */
bool _is_maze_deadend(struct MapCell* cell, struct MapLocation* loc)
{
    int x = loc->x;
    int y = loc->y;

    if(loc->symbol.sym != '#')
        return false;

    if(x == 0 || y == 0 || x == g_map_cell_width-1 || y == g_map_cell_height-1)
        return false;

    int conn_count = 0;

    if(map_cell_get_location(cell, x-1, y)->symbol.sym == '#' || map_cell_get_location(cell, x-1, y)->symbol.sym == '.')
        conn_count++;

    if(map_cell_get_location(cell, x+1, y)->symbol.sym == '#' || map_cell_get_location(cell, x+1, y)->symbol.sym == '.')
        conn_count++;

    if(map_cell_get_location(cell, x, y+1)->symbol.sym == '#' || map_cell_get_location(cell, x, y+1)->symbol.sym == '.')
        conn_count++;

    if(map_cell_get_location(cell, x, y-1)->symbol.sym == '#' || map_cell_get_location(cell, x, y-1)->symbol.sym == '.')
        conn_count++;

    if(conn_count > 1)
        return false;

    return true;
}

/* Finds and sets the input arg to the first deadend it finds.
 * Returns true if it finds a deadend. */
struct MapLocation* _get_maze_deadend(struct MapCell* cell)
{
    for(int x = 0; x < g_map_cell_width; x++)
    for(int y = 0; y < g_map_cell_height; y++)
    {
        struct MapLocation* loc = map_cell_get_location(cell, x, y);
        if(_is_maze_deadend(cell, loc))
        {
            return loc;
        }
    }

    return NULL;
}

/* Finds the next deadend node next to the given one, and sets the input arg to it.
 * Returns true if it finds a deadend */
struct MapLocation* _get_next_deadend_node(struct MapCell* cell, struct MapLocation* loc)
{
    struct MapLocation* next;
    int x = loc->x;
    int y = loc->y;

    next = map_cell_get_location(cell, x-1, y);
    if(_is_maze_deadend(cell, next))
        return next;

    next = map_cell_get_location(cell, x+1, y);
    if(_is_maze_deadend(cell, next))
        return next;

    next = map_cell_get_location(cell, x, y-1);
    if(_is_maze_deadend(cell, next))
        return next;

    next = map_cell_get_location(cell, x, y+1);
    if(_is_maze_deadend(cell, next))
        return next;

    return NULL;
}

/* Recursively fills in deadend corridors until only connected corridors remain. */
void _back_fill_deadends(struct MapCell* cell, struct MapLocation* loc)
{
    struct MapLocation* next;
    while((next = _get_next_deadend_node(cell, loc)))
    {
        next->symbol.sym = ' ';
        next->pathing_flags = 0;
        _back_fill_deadends(cell, next);
    }
}

/* Picks wall sections on each wall at random until it finds one that has a corridor next to it.
 * It converts that wall section into an entryway into the room.
 * TODO: Generate a door object in the entryway tile */
void _make_doors(struct MapCell* cell)
{
    struct Room* room;

    // TODO: This should equal the size of the perimeter of the maximum sized room, make a constant
    struct MapLocation* connectors[36];

    ListNode* rnode;
    list_for_each(&cell->room_list, rnode)
    {
        room = rnode->data;

        int cidx = 0;
        int x, y;

        // North wall
        x = room->x+1;
        y = room->y;
        if(y-1 > 0)
        {
            for(; x < (room->x + room->w - 1); x++)
            {
                if(map_cell_get_location(cell, x, y-1)->symbol.sym == '#')
                {
                    connectors[cidx] = map_cell_get_location(cell, x, y);
                    cidx++;
                }
            }
        }

        // South wall
        x = room->x+1;
        y = room->y + room->h-1;
        if(y+1 < g_map_cell_height-1)
        {
            for(; x < (room->x + room->w - 1); x++)
            {
                if(map_cell_get_location(cell, x, y+1)->symbol.sym == '#')
                {
                    connectors[cidx] = map_cell_get_location(cell, x, y);
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
                if(map_cell_get_location(cell, x-1, y)->symbol.sym == '#')
                {
                    connectors[cidx] = map_cell_get_location(cell, x, y);
                    cidx++;
                }
            }
        }

        // East wall
        x = room->x + room->w - 1;
        y = room->y+1;
        if(x+1 < g_map_cell_width-1)
        {
            for(; y < (room->y + room->h - 1); y++)
            {
                if(map_cell_get_location(cell, x+1, y)->symbol.sym == '#')
                {
                    connectors[cidx] = map_cell_get_location(cell, x, y);
                    cidx++;
                }
            }
        }

        int which = random_int(0, cidx-1);

        connectors[which]->symbol.sym = '.';
        connectors[which]->pathing_flags |= PATHING_GROUND;
    }
}

/* Generate the maze of corridors, adds entryways into rooms, and fills in the deadends */
void gen_maze(struct MapCell* cell)
{
    struct MapLocation* tmp;

    while((tmp = _get_maze_snode(cell)))
    {
        tmp->symbol.sym = '#';
        tmp->pathing_flags |= PATHING_GROUND;
        _flood_fill_maze(cell, tmp);
    }

    _make_doors(cell);

    while((tmp = _get_maze_deadend(cell)))
    {
        tmp->symbol.sym = ' ';
        tmp->pathing_flags = 0;
        _back_fill_deadends(cell, tmp);
    }
}

static void _gen_open_area(struct MapCell* cell)
{
    int w = random_int(g_map_cell_width/4, g_map_cell_width-1);
    int h = random_int(g_map_cell_height/4, g_map_cell_height-1);
    int x = random_int(0, g_map_cell_width-1-w);
    int y = random_int(0, g_map_cell_height-1-h);

#ifdef DEBUG
    log_format_msg(DEBUG, "Room parameters (x, y, w, h): %d, %d, %d, %d", x, y, w, h);
#endif

    struct Room* room = (struct Room*) malloc(sizeof(struct Room));
    room->x = x;
    room->y = y;
    room->w = w;
    room->h = h;

    list_add(&cell->room_list, room); 

    // Fill in with floor
    for(int tmpx = 0; tmpx < w-1; tmpx++)
    for(int tmpy = 0; tmpy < h-1; tmpy++)
    {
        struct MapLocation* loc = map_cell_get_location(cell, x+tmpx, y+tmpy);
        loc->symbol.sym = ' ';
        loc->symbol.bg = ((tmpx+tmpy) % 2 == 0) ? (struct Colour){10,36,10} : (struct Colour){16, 36, 16};
        loc->pathing_flags |= PATHING_GROUND;
    }
}

/* Call this to generate a map that contains rooms and a load of connecting corridors */
void gen_map_cell(struct MapCell* cell, enum MapType type)
{
    if(type == MAPTYPE_DUNGEON)
    {
        gen_rooms(cell);
        gen_maze(cell);
    }
    else
    {
        _gen_open_area(cell);
    }
}
