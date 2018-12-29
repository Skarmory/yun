#ifndef WEAPON_H
#define WEAPON_H

#include "attack.h"

#define W_LONGSWORD 0
#define W_CLAWS     1
#define W_FISTS     2

#define W_RANGE_START 0
#define W_RANGE_END   3

struct Object;

/**
 * Struct containing data about weaponry
 */
struct Weapon
{
    struct Attack attk[1];

    struct Object* obj;
};

struct Weapon* new_weapon(int weapon_type);
void free_weapon(struct Weapon* weapon);

extern struct Weapon weapons[];
extern char* weapon_names[];

#endif
