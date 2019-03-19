#include "inventory.h"

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

#include "equip.h"
#include "log.h"
#include "map.h"
#include "message.h"
#include "mon.h"
#include "object.h"
#include "player.h"
#include "ui.h"

#define DEFAULT_INVENTORY_SIZE 16

typedef struct _PendingActions
{
    ObjList            to_drop;
    struct Object*     to_equip;
    enum EquipmentSlot to_equip_slot;
} PendingActions;

struct Inventory* new_inventory(void)
{
    struct Inventory* inventory = (struct Inventory*) malloc(sizeof(struct Inventory));

    inventory->capacity = DEFAULT_INVENTORY_SIZE;
    inventory->size = 0;
    list_init(&inventory->obj_list);

    return inventory;
}

void free_inventory(struct Inventory* inventory)
{
    struct Object* curr = list_head(&inventory->obj_list, struct Object, obj_list_entry);
    struct Object* next;
    while(curr)
    {
        next = list_next(curr, struct Object, obj_list_entry);

        list_rm(&curr->obj_list_entry, &inventory->obj_list);
        free_obj(curr);

        curr = next;
    }

    free(inventory);
}

/**
 * Runs some simple checks to make sure that the inventory is still in sync.
 */
bool sanity_check_inventory(struct Inventory* inventory)
{
    bool ret = true;
    if(inventory->size > inventory->capacity)
    {
        log_scheck_fail("Inventory size exceeds capacity");
        ret = false;
    }

    if(inventory->size > 0 && inventory->obj_list.head == NULL)
    {
        log_scheck_fail("Inventory size is >0 but objects is null");
        ret = false;
    }

    int counter = 0;
    struct Object* tmp = list_head(&inventory->obj_list, struct Object, obj_list_entry);
    while(tmp)
    {
        counter++;
        tmp = list_next(tmp, struct Object, obj_list_entry);
    }

    if(counter > inventory->size)
    {
        log_scheck_fail("Number of objects in inventory exceeds inventory size");
        ret = false;
    }
    else if(counter < inventory->size)
    {
        log_scheck_fail("Number of objects in inventory is less than inventory size");
        ret = false;
    }

    return ret;
}

/**
 * Attempt to add an object into the inventory.
 */
bool inventory_add_obj(struct Inventory* inventory, struct Object* obj)
{
    if(inventory->size >= inventory->capacity)
    {
        display_msg_log("Cannot fit any more items.");

        return false;
    }

    list_add(&obj->obj_list_entry, &inventory->obj_list);

    inventory->size++;

    return true;
}

bool inventory_rm_obj(struct Inventory* inventory, struct Object* obj)
{
    if(!inventory_has_obj(inventory, obj))
        return false;

    list_rm(&obj->obj_list_entry, &inventory->obj_list);
    inventory->size--;

    return true;
}

bool inventory_has_obj(struct Inventory* inventory, struct Object* obj)
{
    struct Object* curr = list_head(&inventory->obj_list, struct Object, obj_list_entry);
    while(curr)
    {
        if(curr == obj)
            return true;
        curr = list_next(curr, struct Object, obj_list_entry);
    }

    return false;
}

static bool _input_handled(struct Inventory* inventory, PendingActions* pending_actions, struct Object** highlighted, bool* went)
{
    char in = getch();
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

                    list_rm(&drop->obj_list_entry, &inventory->obj_list);
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

                        pending_actions->to_equip_slot = prompt_choice("Choose slot", &handedness_options);
                    }

                    pending_actions->to_equip = *highlighted;
                    *went = true;
                    return true;
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
        display_fmsg_log("You dropped a %s.", curr->name);

        curr = next;
    }

    // Equip items
    if(pending->to_equip)
    {
        // Unequip item in slot and store back into inventory
        struct Object* unequipped = equipment_unequip_obj(equipment, pending->to_equip_slot);
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

bool manage_inventory(void)
{
    PendingActions pending;
    list_init(&pending.to_drop);
    pending.to_equip = NULL;

    bool went = false;
    struct Object* highlighted = list_head(&you->mon->inventory->obj_list, struct Object, obj_list_entry);

    do
    {
        display_char_inventory(you->mon->inventory, you->mon->equipment, &highlighted);

        if(_input_handled(you->mon->inventory, &pending, &highlighted, &went))
            break;
    }
    while(true);

    _resolve_actions(you->mon->inventory, you->mon->equipment, &pending);

    return went;
}
