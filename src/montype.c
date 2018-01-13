#include "montype.h"
#include "colour.h"
#include <ncurses.h>

#define MON(n, sym, weapon, nhd, shd, stats) \
    {                                 \
        n, sym, weapon, nhd, shd, stats      \
    }

#define WEAPON(w) &weapons[w]

#define SYM(s, fg, bg, attr)\
    s, fg, bg, attr

#define STATS(str, agi, in, spi, sta)\
    str, agi, in, spi, sta

struct MonType mon_type[] =
{
    MON("ghoul", SYM('g', CLR_WHITE, CLR_BLACK, 0), WEAPON(W_CLAWS), 1, 4, STATS(5, 5, 5, 5, 5)),
    MON("player", SYM('@', CLR_WHITE, CLR_BLACK, A_BOLD), WEAPON(W_LONGSWORD), 1, 12, STATS(10, 10, 10, 10, 10))
};
