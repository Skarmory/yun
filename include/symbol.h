#ifndef YUN_SYMBOL_H
#define YUN_SYMBOL_H

#include "defs.h"
#include "term.h"

struct Symbol
{
    char sym;
    Colour fg;
    Colour bg;
    unsigned int attr;
    unsigned int base_fg_idx;
    unsigned int base_bg_idx;
};

extern struct Symbol g_symbol_weapon_metal;
extern struct Symbol g_symbol_weapon_wood;

#endif
