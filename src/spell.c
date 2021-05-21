#include "spell.h"

#include "colour.h"
#include "geom.h"
#include "list.h"
#include "look.h"
#include "map.h"
#include "map_cell.h"
#include "message.h"
#include "monster.h"
#include "player.h"
#include "term.h"

#include <string.h>

struct Spell* g_spell = NULL;
int g_spell_count = 0;

enum SpellCastCommand
{
    SPELL_CAST_COMMAND_CONFIRM = KEYCODE_ENTER,
    SPELL_CAST_COMMAND_CANCEL  = KEYCODE_ESC
};

static void _spell_cast_skewer_visuals(const struct Mon* caster, const struct MapLocation* origin, const struct MapLocation* target, List* loc_cache)
{
    int ox = origin->x; // Line origin (start) coords
    int oy = origin->y;
    int nx = ox; // Line segement next coords
    int ny = oy;
    int tx = target->x; // Line target (end) coords
    int ty = target->y;
    int sx = 0; // Screen coords
    int sy = 0;
    float err = 0.0f;

    // Set the origin segment
    struct MapLocation* loc = map_cell_get_location(map_get_cell_by_world_coord(g_cmap, ox, oy), ox, oy);
    struct Symbol what_is_seen = look_get_symbol(loc, caster);

    map_get_screen_coord_by_world_coord(g_cmap, loc->x, loc->y, &sx, &sy);
    term_draw_symbol(sx, sy, &what_is_seen.fg, &g_colours[CLR_WHITE], A_BLINK_BIT, what_is_seen.sym);

    list_add(loc_cache, loc);

    // Set the in-between line segments
    while(geom_gen_line_increment(ox, oy, tx, ty, &nx, &ny, &err))
    {
        loc = map_cell_get_location(map_get_cell_by_world_coord(g_cmap, nx, ny), nx, ny);
        what_is_seen = look_get_symbol(loc, caster);

        map_get_screen_coord_by_world_coord(g_cmap, loc->x, loc->y, &sx, &sy);
        term_draw_symbol(sx, sy, &what_is_seen.fg, &g_colours[CLR_WHITE], A_BLINK_BIT, what_is_seen.sym);

        list_add(loc_cache, loc);
    }

    // Set the target segment
}

static void _spell_cast_set_visuals(const struct Mon* caster, const struct Spell* spell, const struct MapLocation* location, List* loc_cache)
{
    switch(spell->spatial_type)
    {
        case SPELL_SPATIAL_INSTANTANEOUS:
            break;
        case SPELL_SPATIAL_SKEWER:
        {
            struct MapLocation* caster_loc = map_cell_get_location(map_get_cell_by_world_coord(g_cmap, caster->x, caster->y), caster->x, caster->y);
            _spell_cast_skewer_visuals(caster, caster_loc, location, loc_cache);
            break;
        }
        case SPELL_SPATIAL_MISSILE:
            break;
        case SPELL_SPATIAL_AREA_SQUARE:
            break;
    }
}

static void _spell_cast_unset_visuals(const struct Mon* caster, List* loc_cache)
{
    int sx = 0;
    int sy = 0;
    ListNode* n = NULL, *nn = NULL;
    list_for_each_safe(loc_cache, n, nn)
    {
        struct MapLocation* loc = n->data;

        map_get_screen_coord_by_world_coord(g_cmap, loc->x, loc->y, &sx, &sy);

        struct Symbol what_can_be_seen = look_get_symbol(loc, caster);

        term_draw_symbol(sx, sy, &what_can_be_seen.fg, &what_can_be_seen.bg, A_NONE_BIT, what_can_be_seen.sym);

        list_rm(loc_cache, n);
    }
}

static void _spell_cast_loc_info(struct Mon* caster, struct MapLocation* loc)
{
}

void spell_cast(const struct Spell* spell)
{
    struct Mon* caster = g_you->mon;
    struct MapLocation* loc = map_cell_get_location(map_get_cell_by_world_coord(g_cmap, g_you->mon->x, g_you->mon->y), g_you->mon->x, g_you->mon->y);;

    display_msg_nolog("Move cursor to select a target.");
    clear_msgs();
    flush_msg_buffer();
    term_wait_on_input(); // Just wait for player input so they can see the message

    List loc_cache;
    list_init(&loc_cache);

    bool targetting = true;
    enum SpellCastCommand cmd;
    while(targetting)
    {
        _spell_cast_set_visuals(caster, spell, loc, &loc_cache);
        _spell_cast_loc_info(caster, loc);

        term_refresh();
        clear_msgs();
        flush_msg_buffer();

        struct MapLocation* next = NULL;
        cmd = cursor_free_move(loc, &next);
        _spell_cast_unset_visuals(caster, &loc_cache);

        if(cmd == SPELL_CAST_COMMAND_CONFIRM || cmd == SPELL_CAST_COMMAND_CANCEL)
        {
            targetting = false;
        }

        loc = next;
    }

    list_uninit(&loc_cache);
}

struct Spell* spell_look_up_by_id(const char* id)
{
    for(int idx = 0; idx < g_spell_count; ++idx)
    {
        if(strcmp(g_spell[idx].id, id) == 0)
        {
            return &g_spell[idx];
        }
    }

    return NULL;
}
