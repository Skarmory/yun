#include "weapon.h"

#include <stdlib.h>
#include <string.h>

#define WEAPON(n, attk)\
    { n, attk }

#define ATTKS(a1)\
    { a1 }

#define ATTK(dice, sides)\
    { dice, sides }

struct Weapon* new_weapon(int weapon_type)
{
    struct Weapon* weapon = (struct Weapon*) malloc(sizeof(struct Weapon));

    weapon->name = weapons[weapon_type].name;
    memcpy(weapon->attk, weapons[weapon_type].attk, sizeof(weapons[weapon_type].attk));

    return weapon;
}

void free_weapon(struct Weapon* weapon)
{
    free(weapon);
}

/**
 * Array of all the weapons in the game
 */
struct Weapon weapons[] =
{
    WEAPON("longsword", ATTKS(ATTK(1, 6))),
    WEAPON("claws", ATTKS(ATTK(2, 2)))
};
