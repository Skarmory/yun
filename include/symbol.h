#ifndef NAXX_SYMBOL_H
#define NAXX_SYMBOL_H

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

void init_symbols(void);

extern struct Symbol g_symbol_armour_cloth;
extern struct Symbol g_symbol_armour_leather;
extern struct Symbol g_symbol_armour_mail;
extern struct Symbol g_symbol_armour_plate;
extern struct Symbol g_symbol_weapon_metal;

#endif
