#ifndef YUN_MAP_GEN_MAP_LOCATION_H
#define YUN_MAP_GEN_MAP_LOCATION_H

struct Feature;
struct Terrain;

struct MapGenLocation
{
    int x;
    int y;
    const struct Feature* feature;
    const struct Terrain* terrain;
};

#endif
