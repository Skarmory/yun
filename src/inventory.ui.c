#include "inventory.ui.h"

#include "armour.h"
#include "equip.h"
#include "globals.h"
#include "inventory.h"
#include "map.h"
#include "message.h"
#include "mon.h"
#include "ncurses_ext.h"
#include "object.h"
#include "player.h"
#include "ui.h"

#include <ncurses.h>
#include <stdio.h>
#include <string.h>

#define EXTRA_TEXT_LEN 32

const char c_quit = 'q';

const char* c_equipped_text  = "equipped";
const char* c_main_hand_text = "main hand";
const char* c_off_hand_text  = "off hand";
const char* c_already_wearing_cannot_equip_text = "You are already wearing something in that slot.";
const char* c_already_wearing_cannot_drop_text = "You cannot drop something you are wearing.";

const int c_desc_x         = 36;
const int c_desc_width     = 64;

enum Action
{
    ACTION_QUIT_BIT  = 1,
    ACTION_DROP_BIT  = 1 << 1,
    ACTION_EQUIP_BIT = 1 << 2,
    ACTION_MOVE_BIT  = 1 << 3
};

const int c_allow_mask_read_only     = ACTION_QUIT_BIT | ACTION_MOVE_BIT;
const int c_allow_mask_own_inventory = ACTION_QUIT_BIT | ACTION_DROP_BIT | ACTION_EQUIP_BIT | ACTION_MOVE_BIT;

typedef struct _PendingActions
{
    ObjList            to_drop;
    struct Object*     to_equip;
    enum EquipmentSlot to_equip_slot;
} PendingActions;

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
            snprintf(extra_text_string + idx, EXTRA_TEXT_LEN - idx, ", %s", c_main_hand_text);
        else if(slot == EQUIP_SLOT_OFF_HAND)
            snprintf(extra_text_string + idx, EXTRA_TEXT_LEN - idx, ", %s", c_off_hand_text);

        idx = strlen(extra_text_string);
        snprintf(extra_text_string + idx, EXTRA_TEXT_LEN - idx, ")");
    }
}

static inline int _to_bits(char input)
{
    switch(input)
    {
        case 'q': return ACTION_QUIT_BIT;
        case 'd': return ACTION_DROP_BIT;
        case 'e': return ACTION_EQUIP_BIT;
        case 'j':
        case 'k': return ACTION_MOVE_BIT;
    }

    return 0;
}

static bool _input_handled(struct Inventory* inventory, struct Equipment* equipment, PendingActions* pending_actions, struct Object** highlighted, bool* went, int allow_mask)
{
    char in = getch();

    if((_to_bits(in) & allow_mask) == 0)
        return false;

    switch(in)
    {
        case 'q':
            return true;
        case 'd':
            {
                if(highlighted && *highlighted)
                {
                    struct Object* drop = *highlighted;

                    // Cannot drop equipped armours
                    if(equipment_is_equipped(equipment, drop) && drop->objtype == OBJ_TYPE_ARMOUR)
                    {
                        int len = strlen(c_already_wearing_cannot_drop_text);
                        draw_textbox_border( (screen_cols/2) - len/2, (screen_rows/2)-8, len, 1, c_already_wearing_cannot_drop_text);
                        getch();
                        return false;
                    }

                    if(list_next(*highlighted, struct Object, obj_list_entry))
                        *highlighted = list_next(*highlighted, struct Object, obj_list_entry);
                    else
                        *highlighted = list_prev(*highlighted, struct Object, obj_list_entry);

                    inventory_rm_obj(inventory, drop);
                    list_add(&drop->obj_list_entry, &pending_actions->to_drop);
                }
            }
            break;
        case 'e':
            {
                if(highlighted && *highlighted)
                {
                    switch((*highlighted)->objtype)
                    {
                        case OBJ_TYPE_WEAPON:
                        {
                            char* handedness[2] = { "main hand", "off hand" };
                            char choice = prompt_choice_array("Choose slot", handedness, 2);
                            if(choice == g_key_escape)
                            {
                                *went = false;
                                return false;
                            }

                            pending_actions->to_equip_slot = (choice == 'a') ? EQUIP_SLOT_MAIN_HAND : EQUIP_SLOT_OFF_HAND;
                            pending_actions->to_equip = *highlighted;
                            *went = true;
                            return true;
                        }

                        case OBJ_TYPE_ARMOUR:
                        {
                            pending_actions->to_equip_slot = (*highlighted)->objtype_ptr.armour->slot;
                            pending_actions->to_equip = *highlighted;
                            *went = true;
                            return true;
                        }
                    }
                }
            }
            break;
        case 'j':
            {
                if(highlighted && *highlighted && list_next(*highlighted, struct Object, obj_list_entry))
                    *highlighted = list_next(*highlighted, struct Object, obj_list_entry);
            }
            break;
        case 'k':
            {
                if(highlighted && *highlighted && list_prev(*highlighted, struct Object, obj_list_entry))
                    *highlighted = list_prev(*highlighted, struct Object, obj_list_entry);
            }
            break;
    }

    return false;
}

static void _resolve_actions(struct Inventory* inventory, struct Equipment* equipment, PendingActions* pending)
{
    // Drop items
    struct Object* curr = list_head(&pending->to_drop, struct Object, obj_list_entry);
    struct Object* next;
    while(curr)
    {
        next = list_next(curr, struct Object, obj_list_entry);

        inventory_rm_obj(inventory, curr);
        loc_add_obj(map_get_loc(cmap, you->mon->x, you->mon->y), curr);

        if(equipment_is_equipped(equipment, curr) && curr->objtype == OBJ_TYPE_WEAPON)
        {
            equipment_unequip_obj(equipment, curr);
            display_fmsg_log("You thrown down your %s.", curr->name);
        }
        else
        {
            display_fmsg_log("You dropped a %s.", curr->name);
        }

        curr = next;
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
                        display_fmsg_log("You unequipped your %s.", unequipped->name);
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
                    log_format_msg(DEBUG, "Slot: %d", pending->to_equip_slot);
                    display_msg_log("You are already wearing something there!");
                }

                break;
            }
        }
    }
}

static void _display_inventory(struct Inventory* inventory, struct Equipment* equipment, struct Object** highlighted)
{
    int y;
    int displayable_rows = screen_rows - 4;

    clear();

    y = 0;
    mvprintwa_xy(1, y, A_BOLD, "Inventory");
    y += 2;

    struct Object* obj = list_head(&inventory->obj_list, struct Object, obj_list_entry);

    while(obj && y < displayable_rows)
    {
        char extra_text[EXTRA_TEXT_LEN];
        _create_extra_text(extra_text, obj, equipment);

        if(highlighted && obj == *highlighted)
        {
            mvprintwa_xy(1, y++, COLOR_PAIR(30), "%s %s", obj->name, extra_text);
        }
        else
        {
            mvprintw_xy(1, y++, "%s %s", obj->name, extra_text);
        }

        obj = list_next(obj, struct Object, obj_list_entry);
    }

    if(highlighted && *highlighted)
    {

        y = 0;
        mvprintwa_xy(c_desc_x, y, A_BOLD, "Description");
        y+=2;

        draw_textbox(c_desc_x, y, c_desc_width, 0, (*highlighted)->desc);
    }
}

bool display_inventory_player(void)
{
    PendingActions pending;
    list_init(&pending.to_drop);
    pending.to_equip = NULL;

    bool went = false;
    struct Object* highlighted = list_head(&you->mon->inventory->obj_list, struct Object, obj_list_entry);

    do
    {
        _display_inventory(you->mon->inventory, you->mon->equipment, &highlighted);

        mvprintw_xy(1, screen_rows-1, "q: close inventory / d: drop object / e: equip object");

        if(_input_handled(you->mon->inventory, you->mon->equipment, &pending, &highlighted, &went, c_allow_mask_own_inventory))
            break;
    }
    while(true);

    _resolve_actions(you->mon->inventory, you->mon->equipment, &pending);

    return went;
}

void display_inventory_read_only(struct Mon* mon)
{
    struct Object* highlighted = list_head(&mon->inventory->obj_list, struct Object, obj_list_entry);

    do
    {
        _display_inventory(mon->inventory, mon->equipment, &highlighted);

        mvprintw_xy(1, screen_rows-1, "q: close inventory");

        if(_input_handled(NULL, NULL, NULL, NULL, NULL, c_allow_mask_read_only))
            break;
    }
    while(getch() != c_quit);
}
