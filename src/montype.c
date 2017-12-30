#include "montype.h"
#include "colour.h"
#include <ncurses.h>

#define MON(n, sym, weapon) \
    {                 \
        n, sym      \
    }

#define SYM(s, fg, bg, attr)\
    s, fg, bg, attr

struct MonType mon_type[] =
{
    MON("ghoul", SYM('g', CLR_WHITE, CLR_BLACK, 0), W_LONGSWORD),
    MON("player", SYM('@', CLR_WHITE, CLR_BLACK, A_BOLD), W_CLAWS)
};
