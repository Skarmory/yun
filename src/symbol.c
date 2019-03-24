#include "symbol.h"

#include "colour.h"

#include <ncurses.h>

#define SYM(s, fg, bg, attr) { s, fg, bg, attr }

struct Symbol symbols[] = {
    // Mons
    SYM('g', CLR_DC_1,     CLR_BLACK, 0), // ghoul
    SYM('@', CLR_ALLIANCE, CLR_BLACK, 0), // human
    SYM('d', CLR_ALLIANCE, CLR_BLACK, 0), // dwarf
    SYM('e', CLR_ALLIANCE, CLR_BLACK, 0), // night elf
    SYM('g', CLR_ALLIANCE, CLR_BLACK, 0), // gnome
    SYM('o', CLR_HORDE,    CLR_BLACK, 0), // orc
    SYM('u', CLR_HORDE,    CLR_BLACK, 0), // undead
    SYM('t', CLR_HORDE,    CLR_BLACK, 0), // tauren
    SYM('t', CLR_HORDE,    CLR_BLACK, 0), // troll

    // Weapons
    SYM(')', CLR_METAL,    CLR_BLACK, 0), // longsword
    SYM(')', CLR_NONE,     CLR_BLACK, 0), // claws
    SYM(')', CLR_NONE,     CLR_BLACK, 0), // fists

    // Armours
    SYM('[', CLR_NONE,    CLR_BLACK, 0), // Unarmoured
};
