#include "race.h"

const struct Race races[] =
{
    {
        'h', "human", "human",
        RA_HUMAN,
        FA_ALLIANCE,
    },
    {
        'd', "dwarf", "dwarven",
        RA_DWARF,
        FA_ALLIANCE,
    },
    {
        'e', "night elf", "night elven",
        RA_NELF,
        FA_ALLIANCE,
    },
    {
        'g', "gnome", "gnomish",
        RA_GNOME,
        FA_ALLIANCE,
    },
    {
        'o', "orc", "orcish",
        RA_ORC,
        FA_HORDE,
    },
    {
        'f', "forsaken", "forsaken",
        RA_FORSAKEN,
        FA_HORDE,
    },
    {
        't', "tauren", "tauren",
        RA_TAUREN,
        FA_HORDE,
    },
    {
        'l', "troll", "troll",
        RA_TROLL,
        FA_HORDE,
    }
};
