#include "montype.h"

#include "armour.h"
#include "colour.h"
#include "mon_attr.h"
#include "stats.h"
#include "symbol.h"
#include "weapon.h"

#include <ncurses.h>

#define MON(n, sym, armour, weapon, stats, path) \
    {                                    \
        n, sym, armour, weapon, stats, path      \
    }

#define WEAPON(w) &weapons[w]
#define ARMOUR(a) &armours[a]

#define SYM(idx) &symbols[SYM_MON_OFF + idx]

#define STATS(str, agi, in, spi, sta)\
    str, agi, in, spi, sta

/**
 * Array containing all the mon types in the game
 */
struct MonType mon_type[] =
{
    MON("ghoul",     SYM(MT_GHOUL),    ARMOUR(A_UNARMOURED), WEAPON(W_CLAWS), STATS(5, 5, 5, 5, 5),      GROUND),
    MON("human",     SYM(MT_HUMAN),    ARMOUR(A_UNARMOURED), WEAPON(W_FISTS), STATS(20, 20, 20, 20, 20), GROUND),
    MON("dwarf",     SYM(MT_DWARF),    ARMOUR(A_UNARMOURED), WEAPON(W_FISTS), STATS(22, 16, 19, 19, 23), GROUND),
    MON("night elf", SYM(MT_NELF),     ARMOUR(A_UNARMOURED), WEAPON(W_FISTS), STATS(17, 25, 20, 20, 19), GROUND),
    MON("gnome",     SYM(MT_GNOME),    ARMOUR(A_UNARMOURED), WEAPON(W_FISTS), STATS(15, 23, 22, 20, 19), GROUND),
    MON("orc",       SYM(MT_ORC),      ARMOUR(A_UNARMOURED), WEAPON(W_FISTS), STATS(23, 17, 17, 23, 22), GROUND),
    MON("forsaken",  SYM(MT_FORSAKEN), ARMOUR(A_UNARMOURED), WEAPON(W_FISTS), STATS(19, 18, 18, 25, 21), GROUND),
    MON("tauren",    SYM(MT_TAUREN),   ARMOUR(A_UNARMOURED), WEAPON(W_FISTS), STATS(25, 15, 15, 22, 22), GROUND),
    MON("troll",     SYM(MT_TROLL),    ARMOUR(A_UNARMOURED), WEAPON(W_FISTS), STATS(21, 22, 16, 21, 21), GROUND)
};
