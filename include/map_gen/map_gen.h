#ifndef YUN_MAPGEN_H
#define YUN_MAPGEN_H

enum MapType
{
    MAPTYPE_DUNGEON,
    MAPTYPE_OPEN
};

/**
 * Create a map
 */
void generate_map(void);

#endif
