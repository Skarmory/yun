#ifndef YUN_MAP_GEN_MAZE
#define YUN_MAP_GEN_MAZE

struct MapGenCell;
struct Task;

/* Generate the maze of corridors, adds entryways into rooms, and fills in the deadends */
struct Task* map_gen_maze_async(struct MapGenCell* cell);

#endif
