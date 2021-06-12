#ifndef YUN_INVENTORY_H
#define YUN_INVENTORY_H

#include "list.h"

#include <stdbool.h>

struct Object;

struct Inventory
{
    int capacity;
    int size;

    struct List obj_list;
};

struct Inventory* new_inventory(void);
void free_inventory(struct Inventory* inventory);
bool inventory_add_obj(struct Inventory* inventory, struct Object* obj);
bool inventory_rm_obj(struct Inventory* inventory, struct Object* obj);
bool inventory_has_obj(struct Inventory* inventory, struct Object* obj);

bool inventory_sanity_check(struct Inventory* inventory);

#endif

