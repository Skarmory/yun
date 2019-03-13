#ifndef OBJECT_H
#define OBJECT_H

#include "defs.h"
#include "list.h"

struct Weapon;
struct Armour;
struct Symbol;

#define OBJECT_WEAPON_OFF 0
#define OBJECT_ARMOUR_OFF 3

typedef struct ListEntry ObjList;

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
    char* name;
    struct Symbol* symbol;
    union
    {
        struct Weapon* weapon;
        struct Armour* armour;
    } objtype_ptr;

    enum ObjectType objtype;
    ObjList         obj_list;
};

/**
 * Calls the correct free function depending on object type
 */
void free_obj(struct Object* obj);

#endif
