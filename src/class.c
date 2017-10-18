#include "class.h"
#include "race.h"
#include "colour.h"

#define CLS(name, pl, mhk, allow, smask) \
    {\
        name, pl, mhk, allow, smask\
    }\

struct Class classes[] = 
{
    CLS("druid", "druids", 'd', RA_NELF | RA_TAUREN, CLS_DRUID),
    CLS("hunter", "hunters", 'h', RA_DWARF | RA_NELF | RA_ORC | RA_TAUREN | RA_TROLL, CLS_HUNTER),
    CLS("mage", "magi", 'm', RA_HUMAN | RA_DWARF | RA_GNOME | RA_FORSAKEN | RA_TROLL, CLS_MAGE),
    CLS("rogue", "rogues", 'r', RA_HUMAN | RA_DWARF | RA_NELF | RA_GNOME | RA_ORC | RA_FORSAKEN | RA_TROLL, CLS_ROGUE),
    CLS("paladin", "paladins", 'p', RA_HUMAN | RA_DWARF, CLS_PALADIN),
    CLS("priest", "priests", 'i', RA_HUMAN | RA_DWARF | RA_NELF | RA_FORSAKEN | RA_TROLL, CLS_PRIEST),
    CLS("shaman", "shamans", 's', RA_ORC | RA_TAUREN | RA_TROLL, CLS_SHAMAN),
    CLS("warlock", "warlocks", 'l', RA_HUMAN | RA_GNOME | RA_ORC | RA_FORSAKEN, CLS_WARLOCK),
    CLS("warrior", "warriors", 'w', RA_HUMAN | RA_DWARF | RA_NELF | RA_GNOME | RA_ORC | RA_FORSAKEN | RA_TAUREN | RA_TROLL, CLS_WARRIOR)
};

int get_class_colour(const struct Class* cls)
{
    if(!cls)
        return CLR_DEFAULT;

    if(cls->selfmask == CLS_DRUID)
        return CLR_DRUID;
    else if(cls->selfmask == CLS_HUNTER)
        return CLR_HUNTER;
    else if(cls->selfmask == CLS_MAGE)
        return CLR_MAGE;
    else if(cls->selfmask == CLS_ROGUE)
        return CLR_ROGUE;
    else if(cls->selfmask == CLS_PALADIN)
        return CLR_PALADIN;
    else if(cls->selfmask == CLS_PRIEST)
        return CLR_PRIEST;
    else if(cls->selfmask == CLS_SHAMAN)
        return CLR_SHAMAN;
    else if(cls->selfmask == CLS_WARLOCK)
        return CLR_WARLOCK;
    else if(cls->selfmask == CLS_WARRIOR)
        return CLR_WARRIOR;

    return CLR_DEFAULT;
} 
