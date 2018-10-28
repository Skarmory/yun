#ifndef SYMBOL_H
#define SYMBOL_H

#include "defs.h"

#define SYM_MON_OFF    0
#define SYM_WEAPON_OFF 9

struct Symbol
{
    char sym;
    byte fg;
    byte bg;
    byte attr;
};

extern struct Symbol symbols[];

#endif
