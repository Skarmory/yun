#include "montype.h"
#include "colour.h"

#define MON(n, s, fg) \
    {                 \
        n, s, fg      \
    }

struct MonType mon_type[] =
{
    MON("ghoul", 'g', CLR_WHITE),
    MON("player", '@', CLR_WHITE)
};
