#ifndef YUN_MAP_GEN_MAZE
#define YUN_MAP_GEN_MAZE

struct MapCell;

/* Generate the maze of corridors, adds entryways into rooms, and fills in the deadends */
void map_gen_maze(struct MapCell* cell);

#endif
