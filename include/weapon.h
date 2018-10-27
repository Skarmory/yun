#ifndef WEAPON_H
#define WEAPON_H

#include "attack.h"

#define W_LONGSWORD 0
#define W_CLAWS     1
#define W_FISTS     2

struct Object;

/**
 * Struct containing data about weaponry
 */
struct Weapon
{
    char* name;
    struct Attack attk[1];

    struct Object* obj;
};

struct Weapon* new_weapon(int weapon_type);
void free_weapon(struct Weapon* weapon);

extern struct Weapon weapons[];

#endif
