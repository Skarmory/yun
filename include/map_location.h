#ifndef NAXX_MAP_LOCATION_H
#define NAXX_MAP_LOCATION_H

/*
 * Map locations are the actual tiles/squares in the game world that creatures,
 * objects, and other things can occupy.
 */

#include "list.h"
#include "symbol.h"

#include <stdbool.h>

struct Object;
struct PathNode;
struct Mon;

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
struct MapLocation
{
    int x;
    int y;
    PathingFlags pathing_flags;
    struct PathNode* path_node;
    struct Mon* mon;
    List obj_list;
    struct Symbol symbol;
};

bool loc_add_obj(struct MapLocation* loc, struct Object* obj);
bool loc_rm_obj(struct MapLocation* loc, struct Object* obj);

#endif
