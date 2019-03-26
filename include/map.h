#ifndef NAXX_MAP_H
#define NAXX_MAP_H

#include "defs.h"

#include <stdbool.h>

struct List;
struct Mon;
struct Object;
struct PathNode;
struct Room;

#define MROWS 40
#define MCOLS 80

#define WALKABLE 0x00000001

/**
 * Contains information about a square on the map
 */
struct Location
{
    int x, y;
    int pathing;
    struct PathNode* path_node;
    struct Mon* mon;
    List obj_list;
    char terrain;
};

/**
 * Contains the map data
 */
struct Map
{
    struct Location** locs;
    struct Room** rooms;
    int room_count;
    List mon_list;
};

void display_map(void);

struct Map* new_map(void);
void free_map(struct Map* map);
void map_add_mon(struct Map* map, struct Mon* mon);
bool map_rm_mon(struct Map* map, struct Mon* mon);
bool map_move_mon(struct Map* map, struct Mon* mon, int newx, int newy);
bool map_in_bounds(struct Map* map, int x, int y);
bool map_has_mon(struct Map* map, int x, int y);
bool map_is_pathable(struct Map* map, int x, int y, int path_bits);
bool map_valid_move(struct Map* map, int x, int y, int path_bits);
List* map_get_objects(struct Map* map, int x, int y);

static inline struct Location* map_get_loc(struct Map* map, int x, int y)
{
    return &map->locs[x][y];
}

bool loc_add_obj(struct Location* loc, struct Object* obj);
bool loc_rm_obj(struct Location* loc, struct Object* obj);

extern struct Map* cmap;

#endif
