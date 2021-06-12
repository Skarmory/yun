#include "gameplay.h"

#include "colour.h"
#include "console.h"
#include "gameplay_commands.h"
#include "input_keycodes.h"
#include "log.h"
#include "look.h"
#include "map.h"
#include "map_cell.h"
#include "map_location.h"
#include "message.h"
#include "message_utils.h"
#include "movement.h"
#include "monster.h"
#include "mon_attack.h"
#include "mon_inventory.h"
#include "mon_type.h"
#include "object.h"
#include "player.h"
#include "player_class.h"
#include "spell.h"
#include "ui.h"
#include "ui_inventory.h"
#include "ui_stats.h"
#include "util.h"

#include <stddef.h>

/**
 * Automatically determine the action of a player given a movement command
 *
 * Will attempt to attack a monster if possible, otherwise will attempt to move
 */
static bool _do_smart_action(struct MapLocation* target)
{
    if(target->mon == NULL)
    {
        return move_mon(g_you->mon, target->x, target->y);
    }
    else
    {
        return do_attack_mon_mon(g_you->mon, target->mon);
    }
}

/**
 * Try to pick up an object from the floor
 */
static bool _pick_up_object(void)
{
    struct MapLocation* loc = map_get_location(g_cmap, g_you->mon->x, g_you->mon->y);
    struct List* obj_list = loc_get_objs(loc);

    if(obj_list->head == NULL)
    {
       display_msg_log("There is nothing here.");
       return false;
    }

    //TODO: Give selection from all objects on floor
    struct Object* chosen = (struct Object*)obj_list->head->data;

    // Unlink object from Location
    loc_rm_obj(loc, chosen);

    if(!inventory_add_obj(g_you->mon->inventory, chosen))
    {
        // Failed to add to inventory, relink with Location
        loc_add_obj(loc, chosen);
        return false;
    }

    display_fmsg_log("You picked up a %s.", chosen->name);

    return true;
}

void gameplay_turn(void)
{
    bool end_turn = false;

    while(!end_turn)
    {
        enum GameplayCommand cmd = (enum GameplayCommand)get_key();
        switch(cmd)
        {
            case GAMEPLAY_COMMAND_CAST_SPELL:
            {
                // DEBUG CODE
                struct Spell* spell = &g_spells[0];
                // DEBUG CODE
                spell_cast(spell);
                break;
            }
            case GAMEPLAY_COMMAND_DISPLAY_POSITION:
            {
                display_fmsg_nolog("Current position: %d, %d", g_you->mon->x, g_you->mon->y);
                break;
            }
            case GAMEPLAY_COMMAND_MOVE_LEFT:
            case GAMEPLAY_COMMAND_MOVE_RIGHT:
            case GAMEPLAY_COMMAND_MOVE_UP:
            case GAMEPLAY_COMMAND_MOVE_DOWN:
            case GAMEPLAY_COMMAND_MOVE_LEFT_UP:
            case GAMEPLAY_COMMAND_MOVE_LEFT_DOWN:
            case GAMEPLAY_COMMAND_MOVE_RIGHT_UP:
            case GAMEPLAY_COMMAND_MOVE_RIGHT_DOWN:
            {
                struct MapLocation* you_loc = map_get_location(g_cmap, g_you->mon->x, g_you->mon->y);
                if(!you_loc)
                {
                    break;
                }

                struct MapLocation* target = map_get_location_offset_by_direction(g_cmap, you_loc, cmd);
                if(!target)
                {
                    break;
                }

                end_turn = _do_smart_action(target);
                break;
            }
            case GAMEPLAY_COMMAND_PASS_TURN:
            {
                end_turn = true;
                break;
            }
            case GAMEPLAY_COMMAND_PICK_UP:
            {
                end_turn = _pick_up_object();
                break;
            }
            case GAMEPLAY_COMMAND_DISPLAY_INVENTORY:
            {
                end_turn = display_inventory_player();
                break;
            }
            case GAMEPLAY_COMMAND_DISPLAY_CHARACTER_SCREEN:
            {
                end_turn = character_screen_handler();
                break;
            }
            case GAMEPLAY_COMMAND_NO_SAVE_AND_QUIT:
            {
                if(prompt_yn("Really quit?"))
                {
                    do_quit();
                }
                break;
            }
            case GAMEPLAY_COMMAND_SAVE_AND_QUIT:
            {
                if(prompt_yn("Save and quit? (SAVING NOT IMPLEMENTED YET!)"))
                {
                    do_quit();
                }
                break;
            }
            case GAMEPLAY_COMMAND_LOOK:
            {
                look();
                break;
            }
            case GAMEPLAY_COMMAND_SHOW_CONSOLE:
            {
                console();
                break;
            }
        }

        if(!end_turn)
        {
            clear_msgs();
            flush_msg_buffer();
            display_main_screen();
            term_refresh();
        }
    }
}
