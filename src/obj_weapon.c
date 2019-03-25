#include "obj_weapon.h"

#include "list.h"
#include "object.h"
#include "symbol.h"

#include <stdlib.h>
#include <string.h>

#define WEAPON(attk)\
    { attk, NULL }

#define ATTKS(a1)\
    { a1 }

#define ATTK(dice, sides)\
    { dice, sides }

struct Weapon* new_weapon(int weapon_type)
{
    struct Weapon* weapon = (struct Weapon*) malloc(sizeof(struct Weapon));

    memcpy(weapon->attk, weapons[weapon_type].attk, sizeof(weapons[weapon_type].attk));

    weapon->obj = (struct Object*) malloc(sizeof(struct Object));
    weapon->obj->name = weapon_names[weapon_type];
    weapon->obj->desc = weapon_descs[weapon_type];
    weapon->obj->symbol = &symbols[SYM_WEAPON_OFF + weapon_type];
    weapon->obj->objtype_ptr.weapon = weapon;
    weapon->obj->objtype = OBJ_TYPE_WEAPON;
    list_entry_init(&weapon->obj->obj_list_entry);

    return weapon;
}

void free_weapon(struct Weapon* weapon)
{
    free(weapon->obj);
    free(weapon);
}

/**
 * Array of all the weapons in the game
 */
struct Weapon weapons[] =
{
    WEAPON(ATTKS(ATTK(1, 6))),
    WEAPON(ATTKS(ATTK(2, 2))),
    WEAPON(ATTKS(ATTK(1, 2)))
};

char* weapon_names[] =
{
    "longsword",
    "claws",
    "fists"
};

char* weapon_descs[] =
{
    "A one-handed sword with a long blade. This is a popular choice for many adventurers for its durability, reliability, and simplicity.",
    "The sharp natural weapons of most wild beastss and horrors that stalk the land. Can shred cloth and light armour with ease.",
    "The last resort for most adventurers, but can still be deadly when the owner is trained in pugilism."
};
