#include "attack.h"

#define WEAPON(n, attk)\
    { n, attk }

#define ATTKS(a1)\
    { a1 }

#define ATTK(dice, sides)\
    { dice, sides }

struct Weapon weapons[] =
{
    WEAPON("longsword", ATTKS(ATTK(1, 6))),
    WEAPON("claws", ATTKS(ATTK(2, 2)))
};
