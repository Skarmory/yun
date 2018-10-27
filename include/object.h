#ifndef OBJECT_H
#define OBJECT_H

struct Weapon;
struct Armour;

enum ObjectType
{
    WEAPON,
    ARMOUR
};

/**
 * Wrapper class around general game items.
 *
 * Any item that can be picked up should be wrapped by an Object in order to add/remove it from a mon inventory.
 */
struct Object
{
    union
    {
        struct Weapon* weapon;
        struct Armour* armour;
    } objtype_ptr;

    enum ObjectType objtype;
    struct Object* next;
};

#endif
