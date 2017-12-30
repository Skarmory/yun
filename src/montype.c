#include "montype.h"
#include "colour.h"
#include <ncurses.h>

#define MON(n, sym, weapon, nhd, shd) \
    {                 \
        n, sym, weapon, nhd, shd      \
    }

#define WEAPON(w) &weapons[w]

#define SYM(s, fg, bg, attr)\
    s, fg, bg, attr

struct MonType mon_type[] =
{
    MON("ghoul", SYM('g', CLR_WHITE, CLR_BLACK, 0), WEAPON(W_CLAWS), 1, 4),
    MON("player", SYM('@', CLR_WHITE, CLR_BLACK, A_BOLD), WEAPON(W_LONGSWORD), 1, 12)
};
