#ifndef NAXX_SYMBOL_H
#define NAXX_SYMBOL_H

#include "defs.h"

#define SYM_MON_OFF    0
#define SYM_WEAPON_OFF 9
#define SYM_ARMOUR_OFF 12

struct Symbol
{
    char sym;
    byte fg;
    byte bg;
    unsigned int attr;
};

extern struct Symbol symbols[];

#endif
