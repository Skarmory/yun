#ifndef NAXX_MAPGEN_H
#define NAXX_MAPGEN_H

struct Map;

enum MapType
{
    MAPTYPE_DUNGEON,
    MAPTYPE_OPEN
};

void gen_map(struct Map* map, enum MapType type);

#endif
