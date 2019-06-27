#ifndef NAXX_MAP_H
#define NAXX_MAP_H

#include "defs.h"
#include "mon_attr.h"

#include <stdbool.h>

struct List;
struct Mon;
struct Object;
struct PathNode;
struct Room;

#define MROWS 40
#define MCOLS 80

enum PathingFlag
{
    PATHING_NONE   = 0u,
    PATHING_GROUND = 1u,
    PATHING_WATER  = (1u << 1),
    PATHING_FLYING = (1u << 3),
};

typedef unsigned int PathingFlags;

/**
 * Contains information about a square on the map
 */
struct Location
{
    int x, y;
    PathingFlags pathing_flags;
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
    int width;
    int height;
    struct Location** locs;
    struct Room** rooms;
    int room_count;
    List mon_list;
};

void display_map(void);

struct Map* map_new(int width, int height);
void map_free(struct Map* map);
void map_add_mon(struct Map* map, struct Mon* mon);
bool map_rm_mon(struct Map* map, struct Mon* mon);
bool map_move_mon(struct Map* map, struct Mon* mon, int newx, int newy);
bool map_in_bounds(struct Map* map, int x, int y);
bool map_has_mon(struct Map* map, int x, int y);
bool map_is_pathable(struct Map* map, int x, int y, MonAttrMoveFlags move_flags);
bool map_valid_move(struct Map* map, int x, int y, MonAttrMoveFlags move_flags);
List* map_get_objects(struct Map* map, int x, int y);

static inline struct Location* map_get_loc(struct Map* map, int x, int y)
{
    return &map->locs[x][y];
}

bool loc_add_obj(struct Location* loc, struct Object* obj);
bool loc_rm_obj(struct Location* loc, struct Object* obj);

extern struct Map* cmap;

#endif
