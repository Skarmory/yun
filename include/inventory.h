#ifndef INVENTORY_H
#define INVENTORY_H

#include <stdbool.h>

struct Object;

struct Inventory
{
    int capacity;
    int size;

    struct Object* objects;
};

struct Inventory* new_inventory(void);
void free_inventory(struct Inventory* inventory);
bool sanity_check_inventory(struct Inventory* inventory);

#endif

