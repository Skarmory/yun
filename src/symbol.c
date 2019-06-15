#include "symbol.h"

#include "colour.h"

#define SYM(s, fg, bg, attr) { s, {0}, {0}, attr, fg, bg }

struct Symbol g_symbol_armour_cloth   = SYM('[', CLR_ARMOUR_CLOTH, CLR_DEFAULT, 0);
struct Symbol g_symbol_armour_leather = SYM('[', CLR_ARMOUR_LEATHER, CLR_DEFAULT, 0);
struct Symbol g_symbol_armour_mail    = SYM('[', CLR_ARMOUR_MAIL, CLR_DEFAULT, 0);
struct Symbol g_symbol_armour_plate   = SYM('[', CLR_ARMOUR_PLATE, CLR_DEFAULT, 0);

struct Symbol g_symbol_weapon_metal = SYM(')', CLR_METAL, CLR_DEFAULT, 0);

void init_symbols(void)
{
    g_symbol_armour_cloth.fg = *COL(g_symbol_armour_cloth.base_fg_idx);
    g_symbol_armour_cloth.bg = *COL(g_symbol_armour_cloth.base_bg_idx);

    g_symbol_armour_leather.fg = *COL(g_symbol_armour_leather.base_fg_idx);
    g_symbol_armour_leather.bg = *COL(g_symbol_armour_leather.base_bg_idx);

    g_symbol_armour_mail.fg = *COL(g_symbol_armour_mail.base_fg_idx);
    g_symbol_armour_mail.bg = *COL(g_symbol_armour_mail.base_bg_idx);

    g_symbol_armour_plate.fg = *COL(g_symbol_armour_plate.base_fg_idx);
    g_symbol_armour_plate.bg = *COL(g_symbol_armour_plate.base_bg_idx);

    g_symbol_weapon_metal.fg = *COL(g_symbol_weapon_metal.base_fg_idx);
    g_symbol_weapon_metal.bg = *COL(g_symbol_weapon_metal.base_bg_idx);
}
