#ifndef YUN_ROOM_H
#define YUN_ROOM_H

#include <stdbool.h>

struct Room
{
    int x, y, w, h;
};

bool room_has_location(struct Room* room, int x, int y);
bool room_is_corner(struct Room* room, int x, int y);
bool room_is_wall(struct Room* room, int x, int y);

#endif
