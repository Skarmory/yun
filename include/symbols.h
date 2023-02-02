#ifndef YUN_SYMBOLS_H
#define YUN_SYMBOLS_H

#define SYMBOL_WEAPON_METAL_ID 0
#define SYMBOL_WEAPON_WOOD_ID 1

void           symbols_init(void);
struct Symbol* symbols_get(int id);

#endif
