#include "colour.h"
#include "input_keycodes.h"
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
            display_fmsg_nolog("You see yourself! %s %s %s named %s", msg_a_an(you->mon->type->name), you->mon->type->name, you->cls->name, you->name);
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

struct Symbol _look_get_symbol(struct MapLocation* loc, struct Mon* mon)
{
    if(!mon_can_see(mon, loc->x, loc->y))
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

    return loc->symbol;
}

void _look_set_visuals(struct Mon* mon, struct MapLocation* loc)
{
    int sx = 0;
    int sy = 0;
    map_get_screen_coord_by_world_coord(cmap, loc->x, loc->y, &sx, &sy);

    struct Symbol sym = _look_get_symbol(loc, mon);

    term_draw_symbol(sx, sy, &sym.fg, &g_colours[CLR_WHITE], A_BLINK_BIT, sym.sym);
}

void _look_unset_visuals(struct Mon* mon, struct MapLocation* loc)
{
    int sx = 0;
    int sy = 0;
    map_get_screen_coord_by_world_coord(cmap, loc->x, loc->y, &sx, &sy);

    struct Symbol sym = _look_get_symbol(loc, mon);

    term_draw_symbol(sx, sy, &sym.fg, &sym.bg, A_NONE_BIT, sym.sym);
}

void look(void)
{
    int x = you->mon->x;
    int y = you->mon->y;
    struct MapLocation* loc = NULL;

    display_msg_nolog("Move cursor over a location, press esc to stop looking.");
    clear_msgs();
    flush_msg_buffer();
    term_wait_on_input(); // Just wait for player input so they can see the message

    bool looking = true;
    while(looking)
    {
        loc = map_cell_get_location(map_get_cell_by_world_coord(cmap, x, y), x, y);
        _look_set_visuals(you->mon, loc);
        _look_get_loc_info(you->mon, loc);
        clear_msgs();
        flush_msg_buffer();

        enum LookCommand cmd = get_key();
        switch(cmd)
        {
            case LOOK_COMMAND_STOP:
                looking = false;
                break;
            case LOOK_COMMAND_MOVE_UP:
            case LOOK_COMMAND_MOVE_DOWN:
            case LOOK_COMMAND_MOVE_LEFT:
            case LOOK_COMMAND_MOVE_RIGHT:
            case LOOK_COMMAND_MOVE_LEFT_UP:
            case LOOK_COMMAND_MOVE_LEFT_DOWN:
            case LOOK_COMMAND_MOVE_RIGHT_UP:
            case LOOK_COMMAND_MOVE_RIGHT_DOWN:
                {
                    _look_unset_visuals(you->mon, loc);

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

                    struct MapCell* cell = map_get_cell_by_world_coord(cmap, x + dx, y + dy);
                    if(!cell || !map_cell_is_in_bounds(cell, x + dx, y + dy))
                    {
                        break;
                    }

                    x += dx;
                    y += dy;
                }
            default:
                break;
        }
    }

    _look_unset_visuals(you->mon, loc);
    display_msg_nolog("Stopped looking, back to dungeoneering");
    clear_msgs();
    flush_msg_buffer();
    term_wait_on_input();
}
