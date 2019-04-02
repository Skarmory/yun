#include "symbol.h"

#include "colour.h"

#define SYM(s, fg, bg, attr) { s, {0}, {0}, attr, fg, bg }

struct Symbol symbols[] = {
    // Mons
    SYM('g', CLR_DC_1,     CLR_DEFAULT, 0), // ghoul
    SYM('@', CLR_ALLIANCE, CLR_DEFAULT, 0), // human
    SYM('d', CLR_ALLIANCE, CLR_DEFAULT, 0), // dwarf
    SYM('e', CLR_ALLIANCE, CLR_DEFAULT, 0), // night elf
    SYM('g', CLR_ALLIANCE, CLR_DEFAULT, 0), // gnome
    SYM('o', CLR_HORDE,    CLR_DEFAULT, 0), // orc
    SYM('u', CLR_HORDE,    CLR_DEFAULT, 0), // undead
    SYM('t', CLR_HORDE,    CLR_DEFAULT, 0), // tauren
    SYM('t', CLR_HORDE,    CLR_DEFAULT, 0), // troll

    // Weapons
    SYM(')', CLR_METAL, CLR_DEFAULT, 0), // longsword
    SYM(')', CLR_NONE,  CLR_DEFAULT, 0), // claws
    SYM(')', CLR_NONE,  CLR_DEFAULT, 0), // fists

    // Armours
    SYM('[', CLR_NONE, CLR_DEFAULT, 0), // Unarmoured

    // Cloth
    SYM('[', CLR_LPURPLE, CLR_DEFAULT, A_BOLD), // Magister's
    SYM('[', CLR_LPURPLE, CLR_DEFAULT, A_BOLD), // Magister's
    SYM('[', CLR_LPURPLE, CLR_DEFAULT, A_BOLD), // Magister's
    SYM('[', CLR_LPURPLE, CLR_DEFAULT, A_BOLD), // Magister's
    SYM('[', CLR_LPURPLE, CLR_DEFAULT, A_BOLD), // Magister's
    SYM('[', CLR_LPURPLE, CLR_DEFAULT, A_BOLD), // Magister's

    // Leather
    SYM('[', CLR_LGREY, CLR_DEFAULT, A_BOLD), // Shadowcraft
    SYM('[', CLR_LGREY, CLR_DEFAULT, A_BOLD), // Shadowcraft
    SYM('[', CLR_LGREY, CLR_DEFAULT, A_BOLD), // Shadowcraft
    SYM('[', CLR_LGREY, CLR_DEFAULT, A_BOLD), // Shadowcraft
    SYM('[', CLR_LGREY, CLR_DEFAULT, A_BOLD), // Shadowcraft
    SYM('[', CLR_LGREY, CLR_DEFAULT, A_BOLD), // Shadowcraft

    // Mail
    SYM('[', CLR_BROWN, CLR_DEFAULT, A_BOLD), // Beaststalker's
    SYM('[', CLR_BROWN, CLR_DEFAULT, A_BOLD), // Beaststalker's
    SYM('[', CLR_BROWN, CLR_DEFAULT, A_BOLD), // Beaststalker's
    SYM('[', CLR_BROWN, CLR_DEFAULT, A_BOLD), // Beaststalker's
    SYM('[', CLR_BROWN, CLR_DEFAULT, A_BOLD), // Beaststalker's
    SYM('[', CLR_BROWN, CLR_DEFAULT, A_BOLD), // Beaststalker's

    // Plate
    SYM('[', CLR_METAL, CLR_DEFAULT, A_BOLD), // Valor
    SYM('[', CLR_METAL, CLR_DEFAULT, A_BOLD), // Valor
    SYM('[', CLR_METAL, CLR_DEFAULT, A_BOLD), // Valor
    SYM('[', CLR_METAL, CLR_DEFAULT, A_BOLD), // Valor
    SYM('[', CLR_METAL, CLR_DEFAULT, A_BOLD), // Valor
    SYM('[', CLR_METAL, CLR_DEFAULT, A_BOLD)  // Valor
};

void init_symbols(void)
{
    for(int idx = 0; idx < sizeof(symbols) / sizeof(symbols[0]); ++idx)
    {
        symbols[idx].fg = *COL(symbols[idx].base_fg_idx);
        symbols[idx].bg = *COL(symbols[idx].base_bg_idx);
    }
}
