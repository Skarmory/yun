#ifndef YUN_SYMBOL_H
#define YUN_SYMBOL_H

#include "colour.h"
#include "term.h"

struct Symbol
{
    char sym;
    struct Colour fg;
    struct Colour bg;
    unsigned int attr;
    unsigned int base_fg_idx;
    unsigned int base_bg_idx;
};

extern struct Symbol g_symbol_weapon_metal;
extern struct Symbol g_symbol_weapon_wood;

#endif
