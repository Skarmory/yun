#include "spell_skewer.h"

#include "geom.h"
#include "look.h"
#include "log.h"
#include "map.h"
#include "map_cell.h"
#include "map_location.h"
#include "message.h"
#include "monster.h"
#include "mon_type.h"
#include "player.h"
#include "spell.h"
#include "spell_effect.h"
#include "symbol.h"
#include "term.h"

#include <stddef.h>

enum SpellCastCommand
{
    SPELL_CAST_COMMAND_CONFIRM = KEYCODE_ENTER,
    SPELL_CAST_COMMAND_CANCEL  = KEYCODE_ESC
};

static bool _spell_cast_skewer_is_valid_loc(struct Mon* caster, struct MapLocation* loc)
{
    if(caster->x == loc->x && caster->y == loc->y)
    {
        // Cannot skewer yourself!
        return false;
    }

    if(loc_get_pathing(loc) == PATHING_NONE)
    {
        // TODO: Maybe this is a bit naive? Better solution needed?
        //       idea is to stop it from firing through walls, but this should be a spell condition eventually
        return false;
    }

    return true;
}

// Create a line, checking for validity, and get the targets that this spell will hit
static void _spell_cast_skewer_get_targets(struct Mon* caster, struct MapLocation* origin, struct MapLocation** target, List* loc_cache, List* mon_cache)
{
    struct Line line;
    geom_gen_line(&line, origin->x, origin->y, (*target)->x, (*target)->y);

    list_pop_head(&line.coordinate_list); // Discard the origin of the line (the caster's location)

    geom_dbg_log_line(&line, "spell_cast_skewer_get_targets line");

    // Set the in-between line segments
    ListNode* n = NULL;
    list_for_each(&line.coordinate_list, n)
    {
        struct Coordinate* xy = n->data;
        struct MapLocation* loc = map_get_location(g_cmap, xy->x, xy->y);

        if(!_spell_cast_skewer_is_valid_loc(caster, loc))
        {
            break;
        }

        list_add(loc_cache, loc);
        *target = loc;

        if(loc->mon)
        {
            list_add(mon_cache, loc->mon);
        }
    }

    list_uninit(&line.coordinate_list);
}

static void _spell_cast_skewer_target_info(struct Mon* caster, struct MapLocation* loc)
{
}

// Set the symbols for the casting targetting indicator
static void _spell_cast_skewer_set_visuals(struct Mon* caster, List* loc_cache)
{
    int count = 0;
    ListNode* n = NULL;
    list_for_each(loc_cache, n)
    {
        int sx = -1;
        int sy = -1;
        struct MapLocation* loc = n->data;
        struct Symbol what_is_seen = look_get_symbol(loc, caster);

        map_get_screen_coord_by_world_coord(g_cmap, loc->x, loc->y, &sx, &sy);

        if(count == loc_cache->count - 1)
        {
            // Set the "head"/target segment
            term_draw_symbol(sx, sy, &what_is_seen.fg, &g_colours[CLR_WHITE], A_BLINK_BIT, what_is_seen.sym);
        }
        else
        {
            // Set the "tail" segments
            term_draw_symbol(sx, sy, &what_is_seen.fg, &g_colours[CLR_LGREY], A_BLINK_BIT, what_is_seen.sym);
        }

        ++count;
    }
}

// Take the cached locations and unset the term visuals, then clean the cache out
static void _spell_cast_skewer_unset_visuals(struct Mon* caster, List* loc_cache)
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

void spell_cast_skewer(struct Spell* spell, struct Mon* caster)
{
    struct MapLocation* origin = map_get_location(g_cmap, g_you->mon->x, g_you->mon->y);
    struct MapLocation* target = origin;

    List loc_cache;
    List mon_cache;
    list_init(&loc_cache);
    list_init(&mon_cache);

    bool found_target = false;
    while(!found_target)
    {
        list_uninit(&loc_cache);
        list_uninit(&mon_cache);

        display_msg_nolog("Move cursor to select a target [hjklyubn]. Confirm [ENTER]. Cancel [ESC].");

        _spell_cast_skewer_get_targets(caster, origin, &target, &loc_cache, &mon_cache);
        _spell_cast_skewer_set_visuals(caster, &loc_cache);
        _spell_cast_skewer_target_info(caster, target);

        term_refresh();
        clear_msgs();
        flush_msg_buffer();

        _spell_cast_skewer_unset_visuals(caster, &loc_cache);

        struct MapLocation* next_target = NULL;
        enum SpellCastCommand cmd = cursor_free_move(target, &next_target);
        log_format_msg(LOG_DEBUG, "Spell cast command: %d", cmd);

        if(cmd == SPELL_CAST_COMMAND_CONFIRM)
        {
            found_target = true;
            break;
        }

        if(cmd == SPELL_CAST_COMMAND_CANCEL)
        {
            break;
        }

        if(!_spell_cast_skewer_is_valid_loc(caster, next_target))
        {
            continue;
        }

        target = next_target;
    }

    log_msg(LOG_DEBUG, "Spell got targets:");
    ListNode* n = NULL;
    list_for_each(&mon_cache, n)
    {
        log_format_msg(LOG_DEBUG, "\t%s - %p", ((struct Mon*)n->data)->type->name, n->data);
    }

    if(found_target)
    {
        struct SpellEffectArgs args;
        args.affected_locations = &loc_cache;
        args.affected_mons = &mon_cache;
        list_for_each(&spell->spell_effects, n)
        {
            spell_effect_execute((struct SpellEffect*)n->data, &args);
        }
    }

    list_uninit(&loc_cache);
    list_uninit(&mon_cache);
}
