#include "symbols.h"

#include <scieppend/core/colour.h>
#include <scieppend/core/symbol.h>

#define SYM(ch, fg, bg, basefg, basebg, attr) (struct Symbol){ ch, fg, bg, attr, basefg, basebg }

static struct Symbol _symbols[2];

void symbols_init(void)
{
    _symbols[SYMBOL_WEAPON_METAL_ID] = SYM(')', *COL(CLR_METAL), *COL(CLR_DEFAULT), 0, CLR_METAL, CLR_DEFAULT);
    _symbols[SYMBOL_WEAPON_WOOD_ID] = SYM(')', *COL(CLR_METAL), *COL(CLR_DEFAULT), 0, CLR_WOOD, CLR_DEFAULT);
}

struct Symbol* symbols_get(int id)
{
    return &_symbols[id];
}

#undef SYM
