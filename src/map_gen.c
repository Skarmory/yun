#include "map_gen.h"

#include "log.h"
#include "map.h"
#include "map_location.h"
#include "map_room.h"
#include "util.h"

#include <stdlib.h>

/* Draws a map by drawing square rooms at random locations, and with random dimensions. */
void gen_rooms(struct Map* map)
{
    int attempts = 200;
    map->rooms = (struct Room**)malloc(sizeof(struct Room*) * attempts);

    int rcount = -1;
    for(int i = 0; i < attempts; i++)
    {
        int w = random_int(4, 10);
        int h = random_int(4, 8);

        int x = random_int(0, map->width-1-w);
        int y = random_int(0, map->height-1-h);

        bool gen = true;

        for(int r = 0; r <= rcount; r++)
        {
            struct Room* tmp = map->rooms[r];

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

            map->rooms[rcount] = (struct Room*)malloc(sizeof(struct Room));
            struct Room* room = map->rooms[rcount];

            room->x = x;
            room->y = y;
            room->w = w;
            room->h = h;

            // Draw vertical walls
            for(int tmp = 0; tmp < h; tmp++)
            {
                map->locs[x][y+tmp].terrain = '|';
                map->locs[x][y+tmp].pathing_flags = 0;
                map->locs[x+w-1][y+tmp].terrain = '|';
                map->locs[x+w-1][y+tmp].pathing_flags = 0;
            }

            // Draw horizontal walls
            for(int tmp = 0; tmp < w; tmp++)
            {
                map->locs[x+tmp][y].terrain = '-';
                map->locs[x+tmp][y].pathing_flags = 0;
                map->locs[x+tmp][y+h-1].terrain = '-';
                map->locs[x+tmp][y+h-1].pathing_flags = 0;
            }

            // Fill in with floor
            for(int tmpx = 1; tmpx < w-1; tmpx++)
            for(int tmpy = 1; tmpy < h-1; tmpy++)
            {
                map->locs[x + tmpx][y + tmpy].terrain = '.';
                map->locs[x + tmpx][y + tmpy].pathing_flags |= PATHING_GROUND;
            }
        }
    }

    map->rooms = realloc(map->rooms, sizeof(struct Room*) * (rcount+1));
    map->room_count = rcount + 1;
}

/* Check if a location is a valid starting node for a maze
 * Valid maze starting nodes are solid rock surrounded by 8 other solid rock */
bool _is_maze_snode(struct Map* map, struct MapLocation* loc)
{
    int x = loc->x;
    int y = loc->y;

    if(x == 0 || x == map->width - 1 || y == 0 || y == map->height - 1 || map->locs[x][y].terrain != ' ')
        return false;

    if(map->locs[x-1][y-1].terrain == ' ' && map->locs[x][y-1].terrain == ' ' && map->locs[x+1][y-1].terrain == ' ' &&
       map->locs[x-1][y].terrain   == ' ' && map->locs[x+1][y].terrain == ' ' &&
       map->locs[x-1][y+1].terrain == ' ' && map->locs[x][y+1].terrain == ' ' && map->locs[x+1][y+1].terrain == ' ')
        return true;

    return false;
}

/* Finds a maze starting node */
struct MapLocation* _get_maze_snode(struct Map* map)
{
    for(int x = 0; x < map->width; x++)
    for(int y = 0; y < map->height; y++)
    {
        if(_is_maze_snode(map, &map->locs[x][y]))
        {
            return &(map->locs[x][y]);
        }
    }

    return NULL;
}

/* Checks to see if a location is a viable maze node
 * Valid maze nodes need to have only 1 connection to another corridor tile.
 * They can also only have 2 of 4 diagonal squares as corridor tiles so long as those tiles
 * are orthogonal to the single connecting corridor tile. */
bool _is_valid_maze_node(struct Map* map, struct MapLocation* loc)
{
    if(loc->terrain != ' ')
        return false;

    if(loc->x == 0 || loc->y == 0 || loc->x == map->width-1 || loc->y == map->height-1)
        return false;

    int conn_count = 0;

    struct MapLocation* conn;

    if(map->locs[loc->x-1][loc->y].terrain == '#')
    {
        conn_count++;
        conn = &map->locs[loc->x-1][loc->y];
    }

    if(map->locs[loc->x+1][loc->y].terrain == '#')
    {
        conn_count++;
        conn = &map->locs[loc->x+1][loc->y];
    }

    if(map->locs[loc->x][loc->y-1].terrain == '#')
    {
        conn_count++;
        conn = &map->locs[loc->x][loc->y-1];
    }

    if(map->locs[loc->x][loc->y+1].terrain == '#')
    {
        conn_count++;
        conn = &map->locs[loc->x][loc->y+1];
    }

    if(conn_count > 1)
        return false;

    int xoff = loc->x - conn->x;
    int yoff = loc->y - conn->y;

    if(xoff == -1)
    {
        if(map->locs[loc->x-1][loc->y-1].terrain == '#' ||
           map->locs[loc->x-1][loc->y+1].terrain == '#')
            return false;
    }
    else if(xoff == 1)
    {
        if(map->locs[loc->x+1][loc->y-1].terrain == '#' ||
           map->locs[loc->x+1][loc->y+1].terrain == '#')
            return false;
    }
    else if(yoff == 1)
    {
        if(map->locs[loc->x-1][loc->y+1].terrain == '#' ||
           map->locs[loc->x+1][loc->y+1].terrain == '#')
            return false;
    }
    else
    {
        if(map->locs[loc->x-1][loc->y-1].terrain == '#' ||
           map->locs[loc->x+1][loc->y-1].terrain == '#')
            return false;
    }

   return true;
}

/* Attempts to get a location at random orthogonal to a maze node to continue the maze with.
 * It will fall back to just checking every single orthogonal direction if the randomly picked
 * direction is not valid. */
struct MapLocation* _get_valid_maze_node(struct Map* map, struct MapLocation* loc)
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
    next = &map->locs[x + xoff][y + yoff];
    if(_is_valid_maze_node(map, next))
        return next;

    next = &map->locs[x+1][y];
    if(_is_valid_maze_node(map, next))
        return next;

    next = &map->locs[x-1][y];
    if(_is_valid_maze_node(map, next))
        return next;

    next = &map->locs[x][y+1];
    if(_is_valid_maze_node(map, next))
        return next;

    next = &map->locs[x][y-1];
    if(_is_valid_maze_node(map, next))
        return next;

    return NULL;
}

/* Recursive algorithm to carve out a maze in the level. */
void _flood_fill_maze(struct Map* map, struct MapLocation* loc)
{
    struct MapLocation* next;

    while((next = _get_valid_maze_node(map, loc)))
    {
        next->terrain = '#';
        next->pathing_flags |= PATHING_GROUND;
        _flood_fill_maze(map, next);
    }
}

/* Checks if a given location is a deadend.
 * Deadends have only one connecting corridor tile */
bool _is_maze_deadend(struct Map* map, struct MapLocation* loc)
{
    int x = loc->x;
    int y = loc->y;

    if(loc->terrain != '#')
        return false;

    if(x == 0 || y == 0 || x == map->width-1 || y == map->height-1)
        return false;

    int conn_count = 0;

    if(map->locs[x-1][y].terrain == '#' || map->locs[x-1][y].terrain == '.')
        conn_count++;

    if(map->locs[x+1][y].terrain == '#' || map->locs[x+1][y].terrain == '.')
        conn_count++;

    if(map->locs[x][y+1].terrain == '#' || map->locs[x][y+1].terrain == '.')
        conn_count++;

    if(map->locs[x][y-1].terrain == '#' || map->locs[x][y-1].terrain == '.')
        conn_count++;

    if(conn_count > 1)
        return false;

    return true;
}

/* Finds and sets the input arg to the first deadend it finds.
 * Returns true if it finds a deadend. */
struct MapLocation* _get_maze_deadend(struct Map* map)
{
    for(int x = 0; x < map->width; x++)
    for(int y = 0; y < map->height; y++)
    {
        if(_is_maze_deadend(map, &map->locs[x][y]))
        {
            return &map->locs[x][y];
        }
    }

    return NULL;
}

/* Finds the next deadend node next to the given one, and sets the input arg to it.
 * Returns true if it finds a deadend */
struct MapLocation* _get_next_deadend_node(struct Map* map, struct MapLocation* loc)
{
    struct MapLocation* next;
    int x = loc->x;
    int y = loc->y;

    next = &map->locs[x-1][y];
    if(_is_maze_deadend(map, next))
        return next;

    next = &map->locs[x+1][y];
    if(_is_maze_deadend(map, next))
        return next;

    next = &map->locs[x][y-1];
    if(_is_maze_deadend(map, next))
        return next;

    next = &map->locs[x][y+1];
    if(_is_maze_deadend(map, next))
        return next;

    return NULL;
}

/* Recursively fills in deadend corridors until only connected corridors remain. */
void _back_fill_deadends(struct Map* map, struct MapLocation* loc)
{
    struct MapLocation* next;
    while((next = _get_next_deadend_node(map, loc)))
    {
        next->terrain = ' ';
        next->pathing_flags = 0;
        _back_fill_deadends(map, next);
    }
}

/* Picks wall sections on each wall at random until it finds one that has a corridor next to it.
 * It converts that wall section into an entryway into the room.
 * TODO: Generate a door object in the entryway tile */
void _make_doors(struct Map* map)
{
    struct Room* room;

    struct MapLocation** connectors = (struct MapLocation**) malloc(sizeof(struct MapLocation*) * (10 * 2 + 8 * 2));

    for(int i = 0; i < map->room_count; i++)
    {
        room = map->rooms[i];

        int cidx = 0;
        int x, y;

        // North wall
        x = room->x+1;
        y = room->y;
        if(y-1 > 0)
        {
            for(; x < (room->x + room->w - 1); x++)
            {
                if(map->locs[x][y-1].terrain == '#')
                {
                    connectors[cidx] = &map->locs[x][y];
                    cidx++;
                }
            }
        }

        // South wall
        x = room->x+1;
        y = room->y + room->h-1;
        if(y+1 < map->height-1)
        {
            for(; x < (room->x + room->w - 1); x++)
            {
                if(map->locs[x][y+1].terrain == '#')
                {
                    connectors[cidx] = &map->locs[x][y];
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
                if(map->locs[x-1][y].terrain == '#')
                {
                    connectors[cidx] = &map->locs[x][y];
                    cidx++;
                }
            }
        }

        // East wall
        x = room->x + room->w - 1;
        y = room->y+1;
        if(x+1 < map->width-1)
        {
            for(; y < (room->y + room->h - 1); y++)
            {
                if(map->locs[x+1][y].terrain == '#')
                {
                    connectors[cidx] = &map->locs[x][y];
                    cidx++;
                }
            }
        }

        int which = random_int(0, cidx-1);

        connectors[which]->terrain = '.';
        connectors[which]->pathing_flags |= PATHING_GROUND;
    }

    free(connectors);
}

/* Generate the maze of corridors, adds entryways into rooms, and fills in the deadends */
void gen_maze(struct Map* map)
{
    struct MapLocation* tmp;

    while((tmp = _get_maze_snode(map)))
    {
        tmp->terrain = '#';
        tmp->pathing_flags |= PATHING_GROUND;
        _flood_fill_maze(map, tmp);
    }

    _make_doors(map);

    while((tmp = _get_maze_deadend(map)))
    {
        tmp->terrain = ' ';
        tmp->pathing_flags = 0;
        _back_fill_deadends(map, tmp);
    }
}

static void _gen_open_area(struct Map* map)
{
    map->rooms = (struct Room**)malloc(sizeof(struct Room*));
    map->room_count = 1;

    int w = random_int(map->width/4, map->width-1);
    int h = random_int(map->width/4, map->height-1);
    int x = random_int(0, map->width-1-w);
    int y = random_int(0, map->height-1-h);

    map->rooms[0] = (struct Room*)malloc(sizeof(struct Room));
    struct Room* room = map->rooms[0];
    room->x = x;
    room->y = y;
    room->w = w;
    room->h = h;

    // Fill in with floor
    for(int tmpx = 1; tmpx < w-1; tmpx++)
    for(int tmpy = 1; tmpy < h-1; tmpy++)
    {
        map->locs[x + tmpx][y + tmpy].terrain = '.';
        map->locs[x + tmpx][y + tmpy].pathing_flags |= PATHING_GROUND;
    }
}

/* Call this to generate a map that contains rooms and a load of connecting corridors */
void gen_map(struct Map* map, enum MapType type)
{
    if(type == MAPTYPE_DUNGEON)
    {
        gen_rooms(map);
        gen_maze(map);
    }
    else
    {
        _gen_open_area(map);
    }
}
