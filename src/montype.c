#include "montype.h"
#include "stats.h"
#include "colour.h"
#include <ncurses.h>

#define MON(n, sym, weapon, stats, pri, sec, ter) \
    {                                 \
        n, sym, weapon, stats, pri, sec, ter      \
    }

#define WEAPON(w) &weapons[w]

#define SYM(s, fg, bg, attr)\
    s, fg, bg, attr

#define STATS(str, agi, in, spi, sta)\
    str, agi, in, spi, sta

struct MonType mon_type[] =
{
    MON("ghoul", SYM('g', CLR_WHITE, CLR_BLACK, 0), WEAPON(W_CLAWS), STATS(5, 5, 5, 5, 5), AGILITY, STRENGTH, STAMINA),
    MON("player", SYM('@', CLR_WHITE, CLR_BLACK, A_BOLD), WEAPON(W_LONGSWORD), STATS(10, 10, 10, 10, 10), STRENGTH, STAMINA, AGILITY)
};
