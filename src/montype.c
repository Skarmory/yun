#include "montype.h"

#include "armour.h"
#include "colour.h"
#include "mon_attr.h"
#include "stats.h"
#include "weapon.h"

#include <ncurses.h>

#define MON(n, sym, armour, weapon, stats, path) \
    {                                    \
        n, sym, armour, weapon, stats, path      \
    }

#define WEAPON(w) &weapons[w]
#define ARMOUR(a) &armours[a]

#define SYM(s, fg, bg, attr)\
    s, fg, bg, attr

#define STATS(str, agi, in, spi, sta)\
    str, agi, in, spi, sta

/**
 * Array containing all the mon types in the game
 */
struct MonType mon_type[] =
{
    MON("ghoul", SYM('g', CLR_WHITE, CLR_BLACK, 0), ARMOUR(A_UNARMOURED), WEAPON(W_CLAWS), STATS(5, 5, 5, 5, 5), GROUND),
    MON("player", SYM('@', CLR_WHITE, CLR_BLACK, A_BOLD), ARMOUR(A_UNARMOURED), WEAPON(W_LONGSWORD), STATS(50, 50, 50, 50, 50), GROUND)
};
