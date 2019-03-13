#include "inventory.h"

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "message.h"
#include "mon.h"
#include "object.h"
#include "player.h"
#include "ui.h"

#define DEFAULT_INVENTORY_SIZE 16

struct Inventory* new_inventory(void)
{
    struct Inventory* inventory = (struct Inventory*) malloc(sizeof(struct Inventory));

    inventory->capacity = DEFAULT_INVENTORY_SIZE;
    inventory->size = 0;
    inventory->objects = NULL;

    return inventory;
}

void free_inventory(struct Inventory* inventory)
{
    struct Object* curr = inventory->objects;
    struct Object* next;
    while(curr)
    {
        next = list_next(curr, struct Object, obj_list);

        list_rm(curr, obj_list);
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

    if(inventory->size > 0 && inventory->objects == NULL)
    {
        log_scheck_fail("Inventory size is >0 but objects is null");
        ret = false;
    }

    int counter = 0;
    struct Object* tmp = inventory->objects;
    while(tmp)
    {
        counter++;
        tmp = list_next(tmp, struct Object, obj_list);
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

    display_fmsg_log("You picked up a %s.", obj->name);

    list_add(obj, inventory->objects, obj_list);
    inventory->objects = obj;

    inventory->size++;

    return true;
}

bool manage_inventory(void)
{
    bool went = false;

    struct UIList list;
    list.head = you->mon->inventory->objects;
    list.count = you->mon->inventory->size;
    list.current_selection = list.head;

    char input;
    do
    {
        display_char_inventory(&list);
        input = getch();
    }
    while(input != 'q');

    return went;
}
