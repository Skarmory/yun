#include "look.h"

#include "colour.h"
#include "feature.h"
#include "log.h"
#include "map.h"
#include "map_cell.h"
#include "map_location.h"
#include "message.h"
#include "message_utils.h"
#include "monster.h"
#include "mon_type.h"
#include "object.h"
#include "player.h"
#include "player_class.h"
#include "term.h"

#include <stddef.h>

enum LookCommand
{
    LOOK_COMMAND_STOP            = KEYCODE_ESC,
    LOOK_COMMAND_CONFIRM         = KEYCODE_ENTER,
    LOOK_COMMAND_MOVE_LEFT       = KEYCODE_h,
    LOOK_COMMAND_MOVE_RIGHT      = KEYCODE_l,
    LOOK_COMMAND_MOVE_UP         = KEYCODE_k,
    LOOK_COMMAND_MOVE_DOWN       = KEYCODE_j,
    LOOK_COMMAND_MOVE_LEFT_UP    = KEYCODE_y,
    LOOK_COMMAND_MOVE_LEFT_DOWN  = KEYCODE_b,
    LOOK_COMMAND_MOVE_RIGHT_UP   = KEYCODE_u,
    LOOK_COMMAND_MOVE_RIGHT_DOWN = KEYCODE_n
};

void _look_get_loc_info(struct Mon* mon, struct MapLocation* loc)
{
    if(!loc->seen)
    {
        display_msg_nolog("Unexplored area");
        return;
    }

    if(!mon_can_see(mon, loc->x, loc->y))
    {
        display_msg_nolog("You cannot see here.");
        return;
    }

    if(loc->mon)
    {
        if(mon_is_player(loc->mon))
        {
            display_fmsg_nolog("You see yourself! %s %s %s named %s", msg_a_an(g_you->mon->type->name), g_you->mon->type->name, g_you->cls->name, g_you->name);
        }
        else
        {
            display_fmsg_nolog("You see %s %s", msg_a_an(loc->mon->type->name), loc->mon->type->name);
        }
        return;
    }

    if(loc_has_obj(loc))
    {
        const char* obj_name = loc_get_obj(loc)->name;
        display_fmsg_nolog("You see %s %s", msg_a_an(obj_name), obj_name);
        return;
    }
}

struct Symbol look_get_symbol(const struct MapLocation* loc, const struct Mon* looker)
{
    if(!mon_can_see(looker, loc->x, loc->y))
    {
        struct Symbol retsym;

        if(loc->seen)
        {
            retsym.sym = loc->symbol.sym;
            retsym.fg  = *COL(CLR_FOG_OF_WAR);
            retsym.bg  = *COL(CLR_DEFAULT);
        }
        else
        {
            retsym.sym = KEYCODE_SPACE;
            retsym.fg  = *COL(CLR_DEFAULT);
            retsym.bg  = *COL(CLR_DEFAULT);
        }

        return retsym;
    }

    if(loc->mon)
    {
        return *loc->mon->type->symbol;
    }

    if(!list_empty(&loc->obj_list))
    {
        return *((struct Object*)list_peek_head(&loc->obj_list))->symbol;
    }

    if(loc->feature)
    {
        return *loc->feature->symbol;
    }

    return loc->symbol;
}

void _look_set_visuals(struct Mon* mon, struct MapLocation* loc)
{
    int sx = 0;
    int sy = 0;
    map_get_screen_coord_by_world_coord(g_cmap, loc->x, loc->y, &sx, &sy);

    struct Symbol sym = look_get_symbol(loc, mon);

    if(colour_equal(&sym.fg, &g_colours[CLR_WHITE]))
    {
        term_draw_symbol(sx, sy, &g_colours[CLR_BLACK], &g_colours[CLR_WHITE], A_BLINK_BIT, sym.sym);
    }
    else
    {
        term_draw_symbol(sx, sy, &sym.fg, &g_colours[CLR_WHITE], A_BLINK_BIT, sym.sym);
    }
}

void _look_unset_visuals(struct Mon* mon, struct MapLocation* loc)
{
    int sx = 0;
    int sy = 0;
    map_get_screen_coord_by_world_coord(g_cmap, loc->x, loc->y, &sx, &sy);

    struct Symbol sym = look_get_symbol(loc, mon);

    term_draw_symbol(sx, sy, &sym.fg, &sym.bg, A_NONE_BIT, sym.sym);
}

enum KeyCode cursor_free_move(struct MapLocation* in_loc, struct MapLocation** out_loc)
{
    while(true)
    {
        enum LookCommand cmd = get_key();
        switch(cmd)
        {
            case LOOK_COMMAND_CONFIRM:
            case LOOK_COMMAND_STOP:
            {
                return cmd;
            }
            case LOOK_COMMAND_MOVE_UP:
            case LOOK_COMMAND_MOVE_DOWN:
            case LOOK_COMMAND_MOVE_LEFT:
            case LOOK_COMMAND_MOVE_RIGHT:
            case LOOK_COMMAND_MOVE_LEFT_UP:
            case LOOK_COMMAND_MOVE_LEFT_DOWN:
            case LOOK_COMMAND_MOVE_RIGHT_UP:
            case LOOK_COMMAND_MOVE_RIGHT_DOWN:
            {
                int dx = 0;
                int dy = 0;
                if(cmd == LOOK_COMMAND_MOVE_UP || cmd == LOOK_COMMAND_MOVE_LEFT_UP || cmd == LOOK_COMMAND_MOVE_RIGHT_UP)
                {
                    --dy;
                }
                else if(cmd == LOOK_COMMAND_MOVE_DOWN || cmd == LOOK_COMMAND_MOVE_LEFT_DOWN || cmd == LOOK_COMMAND_MOVE_RIGHT_DOWN)
                {
                    ++dy;
                }

                if(cmd == LOOK_COMMAND_MOVE_LEFT || cmd == LOOK_COMMAND_MOVE_LEFT_UP || cmd == LOOK_COMMAND_MOVE_LEFT_DOWN)
                {
                    --dx;
                }
                else if(cmd == LOOK_COMMAND_MOVE_RIGHT || cmd == LOOK_COMMAND_MOVE_RIGHT_UP || cmd == LOOK_COMMAND_MOVE_RIGHT_DOWN)
                {
                    ++dx;
                }

                const int x = in_loc->x + dx;
                const int y = in_loc->y + dy;

                struct MapCell* cell = map_get_cell_by_world_coord(g_cmap, x, y);
                if(!cell || !map_cell_is_in_bounds(cell, x, y))
                {
                    break;
                }

                *out_loc = map_cell_get_location(cell, x, y);
                return cmd;
            }
            default:
            {
                break;
            }
        }
    }

    return LOOK_COMMAND_STOP;
}

void look(void)
{
    display_msg_nolog("Move cursor over a location, press esc to stop looking.");
    clear_msgs();
    flush_msg_buffer();
    term_wait_on_input(); // Just wait for player input so they can see the message

    struct MapLocation* loc = map_get_location(g_cmap, g_you->mon->x, g_you->mon->y);
    struct Mon* mon = g_you->mon;

    bool looking = true;
    while(looking)
    {
        _look_set_visuals(mon, loc);
        _look_get_loc_info(mon, loc);

        term_refresh();
        clear_msgs();
        flush_msg_buffer();

        struct MapLocation* next = NULL;
        enum LookCommand cmd = cursor_free_move(loc, &next);
        _look_unset_visuals(mon, loc);

        if(cmd == LOOK_COMMAND_STOP || cmd == LOOK_COMMAND_CONFIRM)
        {
            looking = false;
        }

        loc = next;
    }

    display_msg_nolog("Stopped looking, back to dungeoneering");
    clear_msgs();
    flush_msg_buffer();
    term_wait_on_input();
}
