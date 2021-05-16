#ifndef YUN_MAPGEN_H
#define YUN_MAPGEN_H

struct Map;
struct MapCell;

enum MapType
{
    MAPTYPE_DUNGEON,
    MAPTYPE_OPEN
};

/* Create a map */
void gen_map(struct Map* map, enum MapType type);

#endif
