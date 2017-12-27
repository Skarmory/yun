#ifndef MAP_H
#define MAP_H

#include <stdbool.h>
#include "mon.h"

struct Location
{
    int x, y;
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
void gen_map(void);
void display_map(void);
void add_mon(struct Mon* mon);
void rm_mon(struct Mon* mon);
bool move_mon(struct Mon* mon, int newx, int newy);

#endif
