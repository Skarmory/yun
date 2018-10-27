#ifndef INVENTORY_H
#define INVENTORY_H

struct Object;

struct Inventory
{
    int capacity;
    int size;

    struct Object** objects;
};

#endif

