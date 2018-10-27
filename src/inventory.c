#include "inventory.h"

#include <stdlib.h>

#include "log.h"
#include "object.h"

struct Inventory* new_inventory(void)
{
    struct Inventory* inventory = (struct Inventory*) malloc(sizeof(struct Inventory));

    inventory->capacity = 0;
    inventory->size = 0;
    inventory->objects = NULL;

    return inventory;
}

void free_inventory(struct Inventory* inventory)
{
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

    struct Object* tmp = inventory->objects;
    int counter = 0;
    while(tmp && tmp->next)
    {
        counter++;
        tmp = tmp->next;
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
