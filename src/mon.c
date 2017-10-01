#include "mon.h"
#include "colours.h"

#define MON(n, s, fg) \
    {                 \
        n, s, fg      \
    }

const struct MonType mon_type[] =
{
    MON("ghoul", 'g', CLR_WHITE),
}
