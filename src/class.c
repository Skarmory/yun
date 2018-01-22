#include "class.h"
#include "race.h"
#include "colour.h"
#include "stats.h"

#define CLS(name, pl, mhk, allow, smask, statups, pst, sst, tst) \
    {\
        name, pl, mhk, allow, smask, statups, pst, sst, tst\
    }

#define STATUPS(str, agi, in, spi, stam) \
    str, agi, in, spi, stam              \


struct Class classes[] = 
{
    CLS("druid", "druids", 'd', RA_NELF | RA_TAUREN, CLS_DRUID,
        STATUPS(2, 2, 2, 1, 1), INTELLIGENCE, SPIRIT, STAMINA),

    CLS("hunter", "hunters", 'h', RA_DWARF | RA_NELF | RA_ORC | RA_TAUREN | RA_TROLL, CLS_HUNTER,
        STATUPS(1, 3, 1, 1, 2), AGILITY, INTELLIGENCE, STAMINA),

    CLS("mage", "magi", 'm', RA_HUMAN | RA_DWARF | RA_GNOME | RA_FORSAKEN | RA_TROLL, CLS_MAGE,
        STATUPS(1, 1, 3, 2, 1), INTELLIGENCE, SPIRIT, STAMINA),

    CLS("rogue", "rogues", 'r', RA_HUMAN | RA_DWARF | RA_NELF | RA_GNOME | RA_ORC | RA_FORSAKEN | RA_TROLL, CLS_ROGUE,
        STATUPS(1, 3, 1, 1, 2), AGILITY, STRENGTH, SPIRIT),

    CLS("paladin", "paladins", 'p', RA_HUMAN | RA_DWARF, CLS_PALADIN,
        STATUPS(2, 1, 1, 1, 3), STRENGTH, STAMINA, INTELLIGENCE),

    CLS("priest", "priests", 'i', RA_HUMAN | RA_DWARF | RA_NELF | RA_FORSAKEN | RA_TROLL, CLS_PRIEST,
        STATUPS(1, 1, 2, 3, 1), SPIRIT, INTELLIGENCE, STAMINA),

    CLS("shaman", "shamans", 's', RA_ORC | RA_TAUREN | RA_TROLL, CLS_SHAMAN,
        STATUPS(2, 1, 3, 1, 1), INTELLIGENCE, SPIRIT, AGILITY),

    CLS("warlock", "warlocks", 'l', RA_HUMAN | RA_GNOME | RA_ORC | RA_FORSAKEN, CLS_WARLOCK,
        STATUPS(1, 1, 3, 1, 2), INTELLIGENCE, STAMINA, SPIRIT),

    CLS("warrior", "warriors", 'w', RA_HUMAN | RA_DWARF | RA_NELF | RA_GNOME | RA_ORC | RA_FORSAKEN | RA_TAUREN | RA_TROLL, CLS_WARRIOR,
        STATUPS(3, 1, 1, 1, 2), STRENGTH, STAMINA, AGILITY)
};

int get_class_colour(const struct Class* cls)
{
    if(!cls)
        return CLR_DEFAULT;

    switch(cls->selfmask)
    {
        case CLS_DRUID:
            return CLR_DRUID;
        case CLS_HUNTER:
            return CLR_HUNTER;
        case CLS_MAGE:
            return CLR_MAGE;
        case CLS_ROGUE:
            return CLR_ROGUE;
        case CLS_PALADIN:
            return CLR_PALADIN;
        case CLS_PRIEST:
            return CLR_PRIEST;
        case CLS_SHAMAN:
            return CLR_SHAMAN;
        case CLS_WARLOCK:
            return CLR_WARLOCK;
        case CLS_WARRIOR:
            return CLR_WARRIOR;
        default:
            return CLR_DEFAULT;
    }


} 
