#include "map_gen/map_gen_room.h"

#include "feature.h"
#include "map_gen/map_gen_cell.h"
#include "map_room.h"
#include "terrain.h"
#include "util.h"

#include <scieppend/core/log.h>
#include <scieppend/core/rng.h>
#include <stddef.h>

// INTERNAL

struct _MapGenRoomState
{
    const struct Feature* hwall;
    const struct Feature* vwall;
    const struct Terrain* floor;
};

static void _gen_room(struct _MapGenRoomState* state, struct MapGenCell* cell)
{
    int w = rng_range(cell->rng, 4, 10);
    int h = rng_range(cell->rng, 4, 8);
    int x = rng_range(cell->rng, cell->world_x, cell->world_x + g_map_cell_width - w - 1);
    int y = rng_range(cell->rng, cell->world_y, cell->world_y + g_map_cell_height - h - 1);

    for(int i = 0; i < array_count(&cell->rooms); ++i)
    {
        struct Room* r = array_get(&cell->rooms, i);
        if((((x >= r->x) && (x <= (r->x + r->w))) || ((r->x >= x) && (r->x <= (x + w)))) &&
           (((y >= r->y) && (y <= (r->y + r->h))) || ((r->y >= y) && (r->y <= (y + h)))))
        {
            return;
        }
    }

    struct Room room;
    room.x = x;
    room.y = y;
    room.w = w;
    room.h = h;

    array_add(&cell->rooms, &room);

    struct MapGenLocation* loc = NULL;

    // Fill in with floor
    for(int xoff = 1; xoff < room.w - 1; ++xoff)
    for(int yoff = 1; yoff < room.h - 1; ++yoff)
    {
        loc = map_gen_cell_get_location(cell, room.x + xoff, room.y + yoff);
        loc->terrain = state->floor;
        loc->feature = NULL;
    }

    // Draw vertical walls
    for(int yoff = 0; yoff < room.h; ++yoff)
    {
        loc = map_gen_cell_get_location(cell, room.x, room.y + yoff);
        loc->feature = state->vwall;

        loc = map_gen_cell_get_location(cell, room.x + room.w - 1, room.y + yoff);
        loc->feature = state->vwall;
    }

    // Draw horizontal walls
    for(int xoff = 0; xoff < room.w; ++xoff)
    {
        loc = map_gen_cell_get_location(cell, room.x + xoff, room.y);
        loc->feature = state->hwall;

        loc = map_gen_cell_get_location(cell, room.x + xoff, room.y + room.h - 1);
        loc->feature = state->hwall;
    }
}

// EXTERNAL

void map_gen_rooms(struct MapGenCell* cell)
{
    struct _MapGenRoomState state;
    state.vwall = feature_look_up_by_id("walv");
    state.hwall = feature_look_up_by_id("walh");
    state.floor = terrain_look_up_by_id("flor");

    // Place as many rooms as we can, limited to 200 attempts
    const int attempts = 200;
    for(int i = 0; i < attempts; ++i)
    {
        _gen_room(&state, cell);
    }
}
