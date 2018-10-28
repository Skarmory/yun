#include "symbol.h"

#include "colour.h"

#define SYM(s, fg, bg, attr) { s, fg, bg, attr }

struct Symbol symbols[] = {
    // Mons
    SYM('g', CLR_DC_1,     CLR_BLACK, 0),
    SYM('@', CLR_ALLIANCE, CLR_BLACK, 0),
    SYM('d', CLR_ALLIANCE, CLR_BLACK, 0),
    SYM('e', CLR_ALLIANCE, CLR_BLACK, 0),
    SYM('g', CLR_ALLIANCE, CLR_BLACK, 0),
    SYM('o', CLR_HORDE,    CLR_BLACK, 0),
    SYM('u', CLR_HORDE,    CLR_BLACK, 0),
    SYM('t', CLR_HORDE,    CLR_BLACK, 0),
    SYM('t', CLR_HORDE,    CLR_BLACK, 0),
};

