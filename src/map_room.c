#include "map_room.h"

#include "geom.h"

bool room_has_location(struct Room* room, int x, int y)
{
    return geom_point_in_rect(x, y, room->x, room->y, room->w-1, room->h-1);
}

bool room_is_corner(struct Room* room, int x, int y)
{
    return ((x == room->x && y == room->y)             ||
            (x == room->x && y == room->y + room->h-1) ||
            (x == room->x + room->w-1 && y == room->y) ||
            (x == room->x + room->w-1 && y == room->y + room->h-1));
}

bool room_is_wall(struct Room* room, int x, int y)
{
    return ((x == room->x && (y >= room->y && y <= room->y + room->h-1)) ||
            (x == room->x + room->w && (y >= room->y && y <= room->y + room->h-1)) ||
            (y == room->y && (x >= room->x && x <= room->x + room->w-1)) ||
            (y == room->y + room->h && (x >= room->x && x <= room->x + room->w-1)));
}
