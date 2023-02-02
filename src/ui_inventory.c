#include "ui_inventory.h"

#include "globals.h"
#include "map.h"
#include "map_cell.h"
#include "map_location.h"
#include "message.h"
#include "monster.h"
#include "mon_equip.h"
#include "mon_inventory.h"
#include "object.h"
#include "obj_armour.h"
#include "player.h"
#include "ui.h"

#include <scieppend/core/colour.h>
#include <scieppend/core/input_keycodes.h>
#include <scieppend/core/log.h>
#include <scieppend/core/term.h>

#include <stdio.h>
#include <string.h>

#define EXTRA_TEXT_LEN 32

const char* c_equipped_text  = "equipped";
const char* c_main_hand_text = "main hand";
const char* c_off_hand_text  = "off hand";
const char* c_already_wearing_cannot_equip_text = "You are already wearing something in that slot.";
const char* c_already_wearing_cannot_drop_text = "You cannot drop something g_you are wearing.";

const int c_list_x = 1;
const int c_list_y = 2;
const int c_list_w = 36;

const int c_desc_x = 36;
const int c_desc_y = 2;
const int c_desc_w = 64;
const int c_desc_h = 5;

enum Action
{
    ACTION_QUIT_BIT  = 1,
    ACTION_DROP_BIT  = 1 << 1,
    ACTION_EQUIP_BIT = 1 << 2,
    ACTION_MOVE_BIT  = 1 << 3
};

enum InventoryCommand
{
    INVENTORY_COMMAND_MOVE_UP   = KEYCODE_k,
    INVENTORY_COMMAND_MOVE_DOWN = KEYCODE_j,
    INVENTORY_COMMAND_EQUIP     = KEYCODE_e,
    INVENTORY_COMMAND_DROP      = KEYCODE_d,
    INVENTORY_COMMAND_QUIT      = KEYCODE_q
};

const int c_allow_mask_read_only     = ACTION_QUIT_BIT | ACTION_MOVE_BIT;
const int c_allow_mask_own_inventory = ACTION_QUIT_BIT | ACTION_DROP_BIT | ACTION_EQUIP_BIT | ACTION_MOVE_BIT;

struct PendingActions
{
    struct List               to_drop;
    struct Object*     to_equip;
    enum EquipmentSlot to_equip_slot;
};

static void _create_extra_text(char* extra_text_string, struct Object* obj, struct Equipment* equipment)
{
    memset(extra_text_string, '\0', EXTRA_TEXT_LEN);
    bool equipped           = equipment_is_equipped(equipment, obj);
    enum EquipmentSlot slot = equipment_slot_by_obj(equipment, obj);

    if(equipped)
    {
        int idx = 0;
        snprintf(extra_text_string, EXTRA_TEXT_LEN, "(%s", c_equipped_text);
        idx = strlen(extra_text_string);

        if(slot == EQUIP_SLOT_MAIN_HAND)
        {
            snprintf(extra_text_string + idx, EXTRA_TEXT_LEN - idx, ", %s", c_main_hand_text);
        }
        else if(slot == EQUIP_SLOT_OFF_HAND)
        {
            snprintf(extra_text_string + idx, EXTRA_TEXT_LEN - idx, ", %s", c_off_hand_text);
        }

        idx = strlen(extra_text_string);
        snprintf(extra_text_string + idx, EXTRA_TEXT_LEN - idx, ")");
    }
}

static inline int _to_bits(char input)
{
    switch(input)
    {
        case INVENTORY_COMMAND_QUIT: return ACTION_QUIT_BIT;
        case INVENTORY_COMMAND_DROP: return ACTION_DROP_BIT;
        case INVENTORY_COMMAND_EQUIP: return ACTION_EQUIP_BIT;
        case INVENTORY_COMMAND_MOVE_DOWN:
        case INVENTORY_COMMAND_MOVE_UP: return ACTION_MOVE_BIT;
    }

    return 0;
}

/**
 * Grab player input here and do something with it.
 * Return true if action should close inventory and end the turn
 */
static bool _input_handled(struct Inventory* inventory, struct Equipment* equipment, struct PendingActions* pending_actions, struct ListNode** highlighted, bool* went, int allow_mask)
{
    enum InventoryCommand in = (enum InventoryCommand)get_key();

    if((_to_bits(in) & allow_mask) == 0)
    {
        // Check if given command is allowed (e.g. cannot drop items if in read-only mode, such as looking at an enemy inventory)
        return false;
    }

    switch(in)
    {
        case INVENTORY_COMMAND_QUIT:
        {
            return true;
        }

        case INVENTORY_COMMAND_DROP:
        {
            if(highlighted && *highlighted)
            {
                struct Object* drop = (*highlighted)->data;

                // Cannot drop equipped armours
                if(equipment_is_equipped(equipment, drop) && drop->objtype == OBJ_TYPE_ARMOUR)
                {
                    int len = strlen(c_already_wearing_cannot_drop_text);
                    int x = (screen_cols / 2) - (len / 2);
                    int y = (screen_rows / 2) - 8;

                    draw_textbox_border(x, y, len, 1, NULL, COL(CLR_DGREY), c_already_wearing_cannot_drop_text);
                    term_refresh();
                    term_wait_on_input();
                    term_clear_area(x, y, len + 4, 5);
                    return false;
                }

                if((*highlighted)->next)
                {
                    *highlighted = (*highlighted)->next;
                }
                else if((*highlighted)->prev)
                {
                    *highlighted = (*highlighted)->prev;
                }
                else
                {
                    *highlighted = NULL;
                }

                term_clear_area(c_list_x, c_list_y, c_desc_x, inventory->obj_list.count + 1);
                inventory_rm_obj(inventory, drop);
                list_add(&pending_actions->to_drop, drop);
            }
            break;
        }

        case INVENTORY_COMMAND_EQUIP:
        {
            if(highlighted && *highlighted)
            {
                struct Object* obj = (struct Object*)(*highlighted)->data;
                switch(obj->objtype)
                {
                    case OBJ_TYPE_WEAPON:
                    {
                        char* handedness[2] = { "main hand", "off hand" };
                        char choice = prompt_choice("Choose slot", handedness, 2);
                        if(choice == KEYCODE_ESC)
                        {
                            *went = false;
                            return false;
                        }

                        pending_actions->to_equip_slot = (choice == 'a') ? EQUIP_SLOT_MAIN_HAND : EQUIP_SLOT_OFF_HAND;
                        pending_actions->to_equip = obj;
                        *went = true;
                        return true;
                    }

                    case OBJ_TYPE_ARMOUR:
                    {
                        pending_actions->to_equip_slot = obj->objtype_ptr.armour->base->slot;
                        pending_actions->to_equip = obj;
                        *went = true;
                        return true;
                    }
                }
            }
            break;
        }

        case INVENTORY_COMMAND_MOVE_DOWN:
        {
            if(highlighted && *highlighted && (*highlighted)->next)
            {
                *highlighted = (*highlighted)->next;
            }
            break;
        }

        case INVENTORY_COMMAND_MOVE_UP:
        {
            if(highlighted && *highlighted && (*highlighted)->prev)
            {
                *highlighted = (*highlighted)->prev;
            }
            break;
        }
    }

    return false;
}

/**
 * If there's an action being performed by the player from the inventory, handle it here.
 */
static void _resolve_actions(struct Inventory* inventory, struct Equipment* equipment, struct PendingActions* pending)
{
    term_clear();

    //struct MapCell* cell = map_get_cell_by_world_coord(g_cmap, g_you->mon->x, g_you->mon->y);

    // Drop items
    struct ListNode* node = NULL;
    list_for_each(&pending->to_drop, node)
    {
        struct Object* obj = node->data;

        inventory_rm_obj(inventory, obj);
        //loc_add_obj(map_cell_get_location(cell, g_you->mon->x, g_you->mon->y), obj);
        loc_add_obj(map_get_location(g_cmap, g_you->mon->x, g_you->mon->y), obj);

        if(equipment_is_equipped(equipment, obj) && obj->objtype == OBJ_TYPE_WEAPON)
        {
            equipment_unequip_obj(equipment, obj);
            display_fmsg_log("You thrown down your %s.", obj->name);
        }
        else
        {
            display_fmsg_log("You dropped a %s.", obj->name);
        }
    }

    // Equip items
    if(pending->to_equip)
    {
        switch(pending->to_equip->objtype)
        {
            case OBJ_TYPE_WEAPON:
            {
                // Unequip item in slot and store back into inventory
                struct Object* unequipped = equipment_unequip_slot(equipment, pending->to_equip_slot);
                bool success = equipment_equip_obj(equipment, pending->to_equip, pending->to_equip_slot);

                if(success)
                {
                    if(unequipped)
                    {
                        display_fmsg_log("You unequipped your %s.", unequipped->name);
                    }
                    display_fmsg_log("You equipped your %s.", pending->to_equip->name);
                }
                else
                {
                    display_fmsg_log("You equipped your %s.",   pending->to_equip->name);

                    // Failed to equip the item, roll back to previous state
                    if(unequipped)
                    {
                        equipment_equip_obj(equipment, unequipped, pending->to_equip_slot);
                    }
                }

                break;
            }

            case OBJ_TYPE_ARMOUR:
            {
                if(equipment_slot_free(equipment, pending->to_equip_slot))
                {
                    equipment_equip_obj(equipment, pending->to_equip, pending->to_equip_slot);
                    display_fmsg_log("You equipped %s.", pending->to_equip->name);
                }
                else
                {
                    log_format_msg(LOG_DEBUG, "Slot: %d", pending->to_equip_slot);
                    display_msg_log("You are already wearing something there!");
                }

                break;
            }
        }
    }
}

/**
 * Internal commonised display inventory method
 */
static void _display_inventory(struct Inventory* inventory, struct Equipment* equipment, struct ListNode** highlighted)
{
    int y;
    int displayable_rows = screen_rows - 4;

    y = 0;
    term_draw_text(1, y, COL(CLR_DEFAULT), COL(CLR_DEFAULT), A_BOLD_BIT, "Inventory");
    y += 2;

    struct ListNode* node = NULL;
    list_for_each(&inventory->obj_list, node)
    {
        if(y >= displayable_rows)
        {
            break;
        }

        char extra_text[EXTRA_TEXT_LEN];
        struct Object* obj = node->data;
        _create_extra_text(extra_text, obj, equipment);

        if(highlighted && node == *highlighted)
        {
            term_draw_ftext(1, y++, COL(CLR_DEFAULT), COL(CLR_DEFAULT), A_BOLD_BIT, "%s %s", obj->name, extra_text);
        }
        else
        {
            term_draw_ftext(1, y++, COL(CLR_DEFAULT), COL(CLR_DEFAULT), 0, "%s %s", obj->name, extra_text);
        }
    }

    y = 0;
    term_draw_text(c_desc_x, y, COL(CLR_DEFAULT), COL(CLR_DEFAULT), A_BOLD_BIT, "Description");
    y+=2;

    if(highlighted && *highlighted)
    {
        draw_textbox(c_desc_x, y, c_desc_w, c_desc_h, NULL, NULL, ((struct Object*)(*highlighted)->data)->desc);
    }
    else
    {
        draw_textbox(c_desc_x, y, c_desc_w, c_desc_h, NULL, NULL, "");
    }
}

/**
 * Call internal display and input handling for player character's inventory
 */
bool display_inventory_player(void)
{
    term_clear();

    struct PendingActions pending;
    list_init(&pending.to_drop);
    pending.to_equip = NULL;

    bool went = false;
    struct ListNode* highlighted = g_you->mon->inventory->obj_list.head;

    do
    {
        _display_inventory(g_you->mon->inventory, g_you->mon->equipment, &highlighted);

        term_draw_ftext(1, screen_rows-1, COL(CLR_DEFAULT), COL(CLR_DEFAULT), 0, "%c: close inventory / %c: drop object / %c: equip object", INVENTORY_COMMAND_QUIT, INVENTORY_COMMAND_DROP, INVENTORY_COMMAND_EQUIP);

        term_refresh();

        if(_input_handled(g_you->mon->inventory, g_you->mon->equipment, &pending, &highlighted, &went, c_allow_mask_own_inventory))
        {
            break;
        }
    }
    while(true);

    _resolve_actions(g_you->mon->inventory, g_you->mon->equipment, &pending);

    return went;
}

/**
 * Call internal display and input handling for some monster's inventory, but with interaction logic disabled
 */
void display_inventory_read_only(struct Mon* mon)
{
    term_clear();

    struct ListNode* highlighted = mon->inventory->obj_list.head;

    do
    {
        _display_inventory(mon->inventory, mon->equipment, &highlighted);

        term_draw_ftext(1, screen_rows-1, COL(CLR_DEFAULT), COL(CLR_DEFAULT), 0, "%c: close inventory", INVENTORY_COMMAND_QUIT);

        term_refresh();

        if(_input_handled(NULL, NULL, NULL, NULL, NULL, c_allow_mask_read_only))
        {
            break;
        }
    }
    while((enum InventoryCommand)get_key() != INVENTORY_COMMAND_QUIT);
}
