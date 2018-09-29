#ifndef ATTACK_DATA_H
#define ATTACK_DATA_H

#define W_LONGSWORD 0
#define W_CLAWS     1

#include "attack.h"

/**
 * Struct containing data about weaponry
 */
struct Weapon
{
    char* name;
    struct Attack attk[1];
};

extern struct Weapon weapons[];

#endif
