#ifndef INVENTORY_H
#define INVENTORY_H

#include "defs.h"

#include <stdbool.h>

struct Object;

struct Inventory
{
    int capacity;
    int size;

    ObjList obj_list;
};

struct Inventory* new_inventory(void);
void free_inventory(struct Inventory* inventory);
bool sanity_check_inventory(struct Inventory* inventory);
bool inventory_add_obj(struct Inventory* inventory, struct Object* obj);
bool inventory_rm_obj(struct Inventory* inventory, struct Object* obj);
bool inventory_has_obj(struct Inventory* inventory, struct Object* obj);

bool manage_inventory(void);

#endif

