#ifndef MAP_H
#define MAP_H

#include <stdbool.h>
#include "mon.h"

#define MROWS 40
#define MCOLS 80

#define WALKABLE 0x00000001

struct Location
{
    int x, y;
    int pathing;
    struct Mon* mon;
    char terrain;
};

struct Room
{
    int x, y, w, h;
};

struct Map
{
    struct Location** locs;
    struct Room** rooms;
    int room_count;
    struct Mon* monlist; // the monsters on this level
};

void init_map(void);
void display_map(void);
void add_mon(struct Mon* mon);
void rm_mon(struct Mon* mon);
bool move_mon(struct Mon* mon, int newx, int newy);

void destroy_map(void);

extern struct Map* cmap;

#endif
