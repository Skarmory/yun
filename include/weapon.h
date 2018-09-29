#ifndef WEAPON_H
#define WEAPON_H

#include "attack.h"

#define W_LONGSWORD 0
#define W_CLAWS     1

/**
 * Struct containing data about weaponry
 */
struct Weapon
{
    char* name;
    struct Attack attk[1];
};

struct Weapon* new_weapon(int weapon_type);
void free_weapon(struct Weapon* weapon);

extern struct Weapon weapons[];

#endif
