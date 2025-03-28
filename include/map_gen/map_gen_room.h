#ifndef YUN_MAP_GEN_MAP_GEN_ROOM_H
#define YUN_MAP_GEN_MAP_GEN_ROOM_H

struct MapGenCell;

/**
 * Draws a map by drawing square rooms at random locations, and with random dimensions.
 */
void map_gen_rooms(struct MapGenCell* cell);

#endif
