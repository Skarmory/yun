#include "montype.h"

#include "colour.h"
#include "mon_attr.h"
#include "stats.h"
#include "weapon.h"

#include <ncurses.h>

#define MON(n, sym, weapon, stats, path) \
    {                                    \
        n, sym, weapon, stats, path      \
    }

#define WEAPON(w) &weapons[w]

#define SYM(s, fg, bg, attr)\
    s, fg, bg, attr

#define STATS(str, agi, in, spi, sta)\
    str, agi, in, spi, sta

/**
 * Array containing all the mon types in the game
 */
struct MonType mon_type[] =
{
    MON("ghoul", SYM('g', CLR_WHITE, CLR_BLACK, 0), WEAPON(W_CLAWS), STATS(5, 5, 5, 5, 5), GROUND),
    MON("player", SYM('@', CLR_WHITE, CLR_BLACK, A_BOLD), WEAPON(W_LONGSWORD), STATS(50, 50, 50, 50, 50), GROUND)
};
