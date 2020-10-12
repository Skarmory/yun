#include "gameplay.h"

#include "command.h"
#include "command_defs.h"
#include "map.h"
#include "map_cell.h"
#include "map_location.h"
#include "message.h"
#include "movement.h"
#include "monster.h"
#include "mon_attack.h"
#include "mon_inventory.h"
#include "object.h"
#include "player.h"
#include "ui.h"
#include "ui_inventory.h"
#include "ui_stats.h"
#include "util.h"

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
    }

    if(!cmd_res->end_turn)
    {
        clear_msgs();
        flush_msg_buffer();
        display_main_screen();
    }
}
