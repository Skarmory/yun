#ifndef MAP_H
#define MAP_H

#include <stdbool.h>

struct Mon;
struct Room;
struct Object;
struct PathNode;

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
    struct Object* objects;
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
    struct Mon* monlist; // the monsters on this level
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
struct Object* map_get_objects(struct Map* map, int x, int y);
int  map_loc_get_neighbours(struct Map* map, struct Location* loc, struct Location*** neighbours);

bool loc_add_obj(struct Location* loc, struct Object* obj);
bool loc_rm_obj(struct Location* loc, struct Object* obj);

extern struct Map* cmap;

#endif
