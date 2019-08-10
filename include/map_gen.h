#ifndef NAXX_MAPGEN_H
#define NAXX_MAPGEN_H

struct Map;
struct MapCell;

enum MapType
{
    MAPTYPE_DUNGEON,
    MAPTYPE_OPEN
};

void gen_map(struct Map* map, enum MapType type);
void gen_map_cell(struct MapCell* cell, enum MapType type);

#endif
