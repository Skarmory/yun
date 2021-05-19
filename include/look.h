#ifndef YUN_LOOK_H
#define YUN_LOOK_H

struct MapLocation;
struct Mon;

struct MapLocation* cursor_free_move(struct MapLocation* loc);
void look(void);

#endif
