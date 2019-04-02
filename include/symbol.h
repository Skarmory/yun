#ifndef NAXX_SYMBOL_H
#define NAXX_SYMBOL_H

#include "defs.h"
#include "term.h"

#define SYM_MON_OFF    0
#define SYM_WEAPON_OFF 9
#define SYM_ARMOUR_OFF 12

struct Symbol
{
    char sym;
    Colour fg;
    Colour bg;
    unsigned int attr;
    unsigned int base_fg_idx;
    unsigned int base_bg_idx;
};

void init_symbols(void);

extern struct Symbol symbols[];

#endif
