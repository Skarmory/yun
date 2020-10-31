#include "symbol.h"

#include "colour.h"

#define SYM(s, fg, bg, attr) { s, {0}, {0}, attr, fg, bg }

struct Symbol g_symbol_weapon_metal = SYM(')', CLR_METAL, CLR_DEFAULT, 0);
struct Symbol g_symbol_weapon_wood = SYM(')', CLR_WOOD, CLR_DEFAULT, 0);
