#ifndef MAP_H
#define MAP_H

struct Map
{
    char** glyphs; // the terrain symbols
    struct Mon** mons;
    struct Mon* monlist; // the monsters on this level
};

void init_map(void);
void display_map(void);

extern struct Map* cmap;

#endif
