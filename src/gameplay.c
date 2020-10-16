#include "gameplay.h"

#include "colour.h"
#include "command.h"
#include "command_defs.h"
#include "input_keycodes.h"
#include "map.h"
#include "map_cell.h"
#include "map_location.h"
#include "message.h"
#include "movement.h"
#include "monster.h"
#include "mon_attack.h"
#include "mon_inventory.h"
#include "mon_type.h"
#include "object.h"
#include "player.h"
#include "ui.h"
#include "ui_inventory.h"
#include "ui_stats.h"
#include "util.h"

enum LookAtCommand
{
    LOOK_AT_COMMAND_STOP            = KEYCODE_ESC,
    LOOK_AT_COMMAND_MOVE_LEFT       = KEYCODE_h,
    LOOK_AT_COMMAND_MOVE_RIGHT      = KEYCODE_l,
    LOOK_AT_COMMAND_MOVE_UP         = KEYCODE_k,
    LOOK_AT_COMMAND_MOVE_DOWN       = KEYCODE_j,
    LOOK_AT_COMMAND_MOVE_LEFT_UP    = KEYCODE_y,
    LOOK_AT_COMMAND_MOVE_LEFT_DOWN  = KEYCODE_b,
    LOOK_AT_COMMAND_MOVE_RIGHT_UP   = KEYCODE_u,
    LOOK_AT_COMMAND_MOVE_RIGHT_DOWN = KEYCODE_n
};

/**
 * Automatically determine the action of a player given a movement command
 *
 * Will attempt to attack a monster if possible, otherwise will attempt to move
 */
static bool _do_smart_action(int x, int y)
{
    // This could be null if it's a movement request to an out of bounds x y
    struct MapCell* cell = map_get_cell_by_world_coord(cmap, x, y);
    if(!cell) return false;

    struct MapLocation* loc = map_cell_get_location(cell, x, y);

    if(loc->mon == NULL)
        return move_mon(you->mon, x, y);
    else
        return do_attack_mon_mon(you->mon, loc->mon);
}

/**
 * Try to pick up an object from the floor
 */
static bool _pick_up_object(void)
{
    struct MapCell* cell = map_get_cell_by_world_coord(cmap, you->mon->x, you->mon->y);
    struct MapLocation* loc = map_cell_get_location(cell, you->mon->x, you->mon->y);
    List* obj_list = map_cell_get_objects(cell, you->mon->x, you->mon->y);

    if(obj_list->head == NULL)
    {
       display_msg_log("There is nothing here.");
       return false;
    }

    //TODO: Give selection from all objects on floor
    struct Object* chosen = (struct Object*)obj_list->head->data;

    // Unlink object from Location
    loc_rm_obj(loc, chosen);

    if(!inventory_add_obj(you->mon->inventory, chosen))
    {
        // Failed to add to inventory, relink with Location
        loc_add_obj(loc, chosen);
        return false;
    }

    display_fmsg_log("You picked up a %s.", chosen->name);

    return true;
}

static void _do_look_at_get_loc_info(struct Mon* mon, struct MapLocation* loc)
{
    if(!mon_can_see(mon, loc->x, loc->y))
    {
        display_msg_nolog("You cannot see here!");
        return;
    }

    if(loc->mon)
    {
        display_fmsg_nolog("You see a(n) %s", loc->mon->type->name);
        return;
    }

    if(loc_has_obj(loc))
    {
        display_fmsg_nolog("You see a(n) %s", loc_get_obj(loc)->name);
        return;
    }
}

static struct Symbol _do_look_at_get_symbol(struct MapLocation* loc, struct Mon* mon)
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

static void _do_look_at_set_visuals(struct Mon* mon, struct MapLocation* loc)
{
    int sx = 0;
    int sy = 0;
    map_get_screen_coord_by_world_coord(cmap, loc->x, loc->y, &sx, &sy);

    struct Symbol sym = _do_look_at_get_symbol(loc, mon);

    term_draw_symbol(sx, sy, &sym.fg, &g_colours[CLR_WHITE], A_BLINK_BIT, sym.sym);
}

static void _do_look_at_unset_visuals(struct Mon* mon, struct MapLocation* loc)
{
    int sx = 0;
    int sy = 0;
    map_get_screen_coord_by_world_coord(cmap, loc->x, loc->y, &sx, &sy);

    struct Symbol sym = _do_look_at_get_symbol(loc, mon);

    term_draw_symbol(sx, sy, &sym.fg, &sym.bg, A_NONE_BIT, sym.sym);
}

static void _do_look_at(void)
{
    int x = you->mon->x;
    int y = you->mon->y;
    struct MapLocation* loc = NULL;

    bool looking = true;
    while(looking)
    {
        loc = map_cell_get_location(map_get_cell_by_world_coord(cmap, x, y), x, y);
        _do_look_at_set_visuals(you->mon, loc);
        _do_look_at_get_loc_info(you->mon, loc);
        clear_msgs();
        flush_msg_buffer();

        enum LookAtCommand cmd = (enum LookAtCommand) term_getch();
        switch(cmd)
        {
            case LOOK_AT_COMMAND_STOP:
                looking = false;
                break;
            case LOOK_AT_COMMAND_MOVE_UP:
            case LOOK_AT_COMMAND_MOVE_DOWN:
            case LOOK_AT_COMMAND_MOVE_LEFT:
            case LOOK_AT_COMMAND_MOVE_RIGHT:
            case LOOK_AT_COMMAND_MOVE_LEFT_UP:
            case LOOK_AT_COMMAND_MOVE_LEFT_DOWN:
            case LOOK_AT_COMMAND_MOVE_RIGHT_UP:
            case LOOK_AT_COMMAND_MOVE_RIGHT_DOWN:
                {
                    _do_look_at_unset_visuals(you->mon, loc);

                    int dx = 0;
                    int dy = 0;
                    if(cmd == LOOK_AT_COMMAND_MOVE_UP || cmd == LOOK_AT_COMMAND_MOVE_LEFT_UP || cmd == LOOK_AT_COMMAND_MOVE_RIGHT_UP)
                    {
                        --dy;
                    }

                    if(cmd == LOOK_AT_COMMAND_MOVE_DOWN || cmd == LOOK_AT_COMMAND_MOVE_LEFT_DOWN || cmd == LOOK_AT_COMMAND_MOVE_RIGHT_DOWN)
                    {
                        ++dy;
                    }

                    if(cmd == LOOK_AT_COMMAND_MOVE_LEFT || cmd == LOOK_AT_COMMAND_MOVE_LEFT_UP || cmd == LOOK_AT_COMMAND_MOVE_LEFT_DOWN)
                    {
                        --dx;
                    }

                    if(cmd == LOOK_AT_COMMAND_MOVE_RIGHT || cmd == LOOK_AT_COMMAND_MOVE_RIGHT_UP || cmd == LOOK_AT_COMMAND_MOVE_RIGHT_DOWN)
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

    _do_look_at_unset_visuals(you->mon, loc);
}

void gameplay_command_handler_func(struct Command* cmd, struct CommandResult* cmd_res)
{
    cmd_res->end_turn = false;

    switch(cmd->type)
    {
        case COMMAND_TYPE_DISPLAY_POSITION:
            display_fmsg_nolog("Current position: %d, %d", you->mon->x, you->mon->y);
            break;
        case COMMAND_TYPE_MOVE:
            {
                char ch = cmd->cmd_char;
                cmd_res->end_turn = _do_smart_action(
                    you->mon->x + (ch == 'h' || ch == 'y' || ch == 'b' ? -1 : ch == 'l' || ch == 'u' || ch == 'n' ? 1 : 0),
                    you->mon->y + (ch == 'k' || ch == 'y' || ch == 'u' ? -1 : ch == 'j' || ch == 'b' || ch == 'n' ? 1 : 0)
                );
            }
            break;
        case COMMAND_TYPE_PASS_TURN:
            cmd_res->end_turn = true;
            break;
        case COMMAND_TYPE_PICK_UP:
            cmd_res->end_turn = _pick_up_object();
            break;
        case COMMAND_TYPE_DISPLAY_INVENTORY:
            cmd_res->end_turn = display_inventory_player();
            break;
        case COMMAND_TYPE_DISPLAY_CHARACTER_SCREEN:
            cmd_res->end_turn = character_screen_handler();
            break;
        case COMMAND_TYPE_NO_SAVE_AND_QUIT:
            if(prompt_yn("Really quit?"))
            {
                do_quit();
            }
            break;
        case COMMAND_TYPE_SAVE_AND_QUIT:
            if(prompt_yn("Save and quit? (SAVING NOT IMPLEMENTED YET!)"))
            {
                do_quit();
            }
            break;
        case COMMAND_TYPE_LOOK_AT:
            _do_look_at();
            break;
    }

    if(!cmd_res->end_turn)
    {
        clear_msgs();
        flush_msg_buffer();
        display_main_screen();
    }
}
