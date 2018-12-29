#include "montype.h"

#include "armour.h"
#include "colour.h"
#include "mon_attr.h"
#include "stats.h"
#include "symbol.h"
#include "weapon.h"

#include <ncurses.h>

#define MON(n, sym, bai, bwi, stats, path) \
    {                                    \
        n, sym, bai, bwi, NULL, NULL, stats, path      \
    }

#define WEAPON(w) &weapons[w]
#define ARMOUR(a) &armours[a]

#define SYM(idx) &symbols[SYM_MON_OFF + idx]

#define STATS(str, agi, in, spi, sta)\
    str, agi, in, spi, sta

/**
 * Do all runtime initialisation for montypes
 */
void init_montypes(void)
{
    for(unsigned int idx = MT_RANGE_START; idx < MT_RANGE_END; idx++)
    {
        // Create base armour and weapons for each mon type
        mon_type[idx].base_armour = new_armour(mon_type[idx].base_armour_idx);
        mon_type[idx].base_weapon = new_weapon(mon_type[idx].base_weapon_idx);
    }
}

/**
 * Array containing all the mon types in the game
 */
struct MonType mon_type[] =
{
    MON("ghoul",     SYM(MT_GHOUL),    A_UNARMOURED, W_CLAWS, STATS(5, 5, 5, 5, 5),      GROUND),
    MON("human",     SYM(MT_HUMAN),    A_UNARMOURED, W_FISTS, STATS(20, 20, 20, 20, 20), GROUND),
    MON("dwarf",     SYM(MT_DWARF),    A_UNARMOURED, W_FISTS, STATS(22, 16, 19, 19, 23), GROUND),
    MON("night elf", SYM(MT_NELF),     A_UNARMOURED, W_FISTS, STATS(17, 25, 20, 20, 19), GROUND),
    MON("gnome",     SYM(MT_GNOME),    A_UNARMOURED, W_FISTS, STATS(15, 23, 22, 20, 19), GROUND),
    MON("orc",       SYM(MT_ORC),      A_UNARMOURED, W_FISTS, STATS(23, 17, 17, 23, 22), GROUND),
    MON("forsaken",  SYM(MT_FORSAKEN), A_UNARMOURED, W_FISTS, STATS(19, 18, 18, 25, 21), GROUND),
    MON("tauren",    SYM(MT_TAUREN),   A_UNARMOURED, W_FISTS, STATS(25, 15, 15, 22, 22), GROUND),
    MON("troll",     SYM(MT_TROLL),    A_UNARMOURED, W_FISTS, STATS(21, 22, 16, 21, 21), GROUND)
};
