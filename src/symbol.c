#include "symbol.h"

#include "colour.h"

#define SYM(s, fg, bg, attr) { s, {0}, {0}, attr, fg, bg }

struct Symbol g_symbol_armour_cloth   = SYM('[', CLR_ARMOUR_CLOTH, CLR_DEFAULT, 0);
struct Symbol g_symbol_armour_leather = SYM('[', CLR_ARMOUR_LEATHER, CLR_DEFAULT, 0);
struct Symbol g_symbol_armour_mail    = SYM('[', CLR_ARMOUR_MAIL, CLR_DEFAULT, 0);
struct Symbol g_symbol_armour_plate   = SYM('[', CLR_ARMOUR_PLATE, CLR_DEFAULT, 0);

struct Symbol g_symbol_weapon_metal = SYM(')', CLR_METAL, CLR_DEFAULT, 0);
struct Symbol g_symbol_weapon_wood = SYM(')', CLR_WOOD, CLR_DEFAULT, 0);
