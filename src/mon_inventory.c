#include "mon_inventory.h"

#include "map.h"
#include "message.h"
#include "monster.h"
#include "mon_equip.h"
#include "object.h"
#include "player.h"
#include "ui.h"

#include <scieppend/core/log.h>

#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_INVENTORY_SIZE 16

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
    struct ListNode *node, *n;
    list_for_each_safe(&inventory->obj_list, node, n)
    {
        free_obj(node->data);
        free(node);
    }

    free(inventory);
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

    list_add(&inventory->obj_list, obj);

    inventory->size++;

    return true;
}

bool inventory_rm_obj(struct Inventory* inventory, struct Object* obj)
{
    struct ListNode* node = list_find(&inventory->obj_list, obj);
    if(!node) return false;

    list_rm(&inventory->obj_list, node);
    inventory->size--;

    return true;
}

bool inventory_has_obj(struct Inventory* inventory, struct Object* obj)
{
    struct ListNode* node;
    list_for_each(&inventory->obj_list, node)
    {
        if(node->data == obj) return true;
    }
    return false;
}

/**
 * Runs some simple checks to make sure that the inventory is still in sync.
 */
bool inventory_sanity_check(struct Inventory* inventory)
{
    bool ret = true;
    if(inventory->size > inventory->capacity)
    {
        log_msg(LOG_DEBUG, "Inventory size exceeds capacity");
        ret = false;
    }

    if(inventory->size > 0 && inventory->obj_list.head == NULL)
    {
        log_msg(LOG_DEBUG, "Inventory size is >0 but objects is null");
        ret = false;
    }

    int counter = 0;
    struct ListNode* node = NULL;
    list_for_each(&inventory->obj_list, node)
    {
        ++counter;
    }

    if(counter > inventory->size)
    {
        log_msg(LOG_DEBUG, "Number of objects in inventory exceeds inventory size");
        ret = false;
    }
    else if(counter < inventory->size)
    {
        log_msg(LOG_DEBUG, "Number of objects in inventory is less than inventory size");
        ret = false;
    }

    return ret;
}
