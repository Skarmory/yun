#include "inventory.ui.h"

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

static bool _input_handled(struct Inventory* inventory, PendingActions* pending_actions, struct Object** highlighted, bool* went, int allow_mask)
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
                    if((*highlighted)->objtype == OBJ_TYPE_WEAPON)
                    {
                        struct List handedness_options;
                        list_init(&handedness_options);

                        struct UIOption main_hand;
                        snprintf(main_hand.option_name, g_option_name_max_size, "%s", "main hand");
                        list_entry_init(&main_hand.option_list_entry);

                        struct UIOption off_hand;
                        snprintf(off_hand.option_name, g_option_name_max_size, "%s", "off hand");
                        list_entry_init(&off_hand.option_list_entry);

                        list_add(&off_hand.option_list_entry, &handedness_options);
                        list_add(&main_hand.option_list_entry, &handedness_options);

                        char choice = prompt_choice("Choose slot", &handedness_options);
                        if(choice == 27)
                        {
                            *went = false;
                            return false;
                        }

                        if(choice == 'a')
                            pending_actions->to_equip_slot = EQUIP_SLOT_MAIN_HAND;
                        else if(choice == 'b')
                            pending_actions->to_equip_slot = EQUIP_SLOT_OFF_HAND;

                        pending_actions->to_equip = *highlighted;
                        *went = true;
                        return true;
                    }

                }
            }
            break;
        case 'j':
            {
                if(highlighted && *highlighted)
                    if(list_next(*highlighted, struct Object, obj_list_entry))
                        *highlighted = list_next(*highlighted, struct Object, obj_list_entry);
            }
            break;
        case 'k':
            {
                if(highlighted && *highlighted)
                    if(list_prev(*highlighted, struct Object, obj_list_entry))
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

        if(_input_handled(you->mon->inventory, &pending, &highlighted, &went, c_allow_mask_own_inventory))
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

        if(_input_handled(NULL, NULL, NULL, NULL, c_allow_mask_read_only))
            break;
    }
    while(getch() != c_quit);
}
