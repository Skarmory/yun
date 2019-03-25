#include "player_class.h"

#include "colour.h"
#include "mon_stats.h"
#include "player_race.h"

#define CLS(name, pl, mhk, allow, smask, statups, statscales) \
    {\
        name, pl, mhk, allow, smask, statups, statscales\
    }

#define STATUPS(str, agi, in, spi, stam) \
    str, agi, in, spi, stam              \

#define STATSCA(str, agi, in, spi, stam) STATUPS(str, agi, in, spi, stam)

/**
 * Array of all the player class types
 */
struct Class classes[] = 
{
    CLS("druid", "druids", 'd', RA_NELF | RA_TAUREN, CLS_DRUID, 
        STATUPS(1, 0, 2, 2, 0), STATSCA(0.5f, 0.5f, 0.75f, 0.5f, 0.75f)),

    CLS("hunter", "hunters", 'h', RA_DWARF | RA_NELF | RA_ORC | RA_TAUREN | RA_TROLL, CLS_HUNTER, 
        STATUPS(0, 3, 0, 1, 1), STATSCA(0.3f, 1.0f, 0.7f, 0.5f, 0.5f)),

    CLS("mage", "magi", 'm', RA_HUMAN | RA_DWARF | RA_GNOME | RA_FORSAKEN | RA_TROLL, CLS_MAGE, 
        STATUPS(0, 0, 3, 2, 0), STATSCA(0.1f, 0.1f, 2.0f, 0.5f, 0.2f)),

    CLS("rogue", "rogues", 'r', RA_HUMAN | RA_DWARF | RA_NELF | RA_GNOME | RA_ORC | RA_FORSAKEN | RA_TROLL, CLS_ROGUE, 
        STATUPS(1, 3, 0, 0, 1), STATSCA(0.5f, 1.5f, 0.35f, 0.35f, 0.3f)),

    CLS("paladin", "paladins", 'p', RA_HUMAN | RA_DWARF, CLS_PALADIN, 
        STATUPS(2, 0, 0, 1, 2), STATSCA(0.75f, 0.35f, 0.75f, 0.45f, 0.75f)),

    CLS("priest", "priests", 'i', RA_HUMAN | RA_DWARF | RA_NELF | RA_FORSAKEN | RA_TROLL, CLS_PRIEST, 
        STATUPS(0, 0, 2, 3, 0), STATSCA(0.15f, 0.25f, 0.65f, 1.5f, 0.5f)),

    CLS("shaman", "shamans", 's', RA_ORC | RA_TAUREN | RA_TROLL, CLS_SHAMAN, 
        STATUPS(1, 0, 1, 2, 1), STATSCA(0.75f, 0.75f, 0.75f, 0.35f, 0.4f)),

    CLS("warlock", "warlocks", 'l', RA_HUMAN | RA_GNOME | RA_ORC | RA_FORSAKEN, CLS_WARLOCK, 
        STATUPS(0, 0, 2, 2, 1), STATSCA(0.15f, 0.25f, 1.5f, 0.6f, 0.5f)),

    CLS("warrior", "warriors", 'w', RA_HUMAN | RA_DWARF | RA_NELF | RA_GNOME | RA_ORC | RA_FORSAKEN | RA_TAUREN | RA_TROLL, CLS_WARRIOR, 
        STATUPS(3, 0, 0, 0, 2), STATSCA(1.0f, 0.5f, 0.1f, 0.4f, 1.0f))
};

/**
 * Return the colour associated with given class based on how WoW addons defined them
 */
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

/**
 * Return class index into class array by given class' unique char
 */
int get_class_idx(char cls)
{
    switch(cls)
    {
        case 'd':
            return CLS_DRUID_IDX;
        case 'h':
            return CLS_HUNTER_IDX;
        case 'm':
            return CLS_MAGE_IDX;
        case 'r':
            return CLS_ROGUE_IDX;
        case 'p':
            return CLS_PALADIN_IDX;
        case 'i':
            return CLS_PRIEST_IDX;
        case 's':
            return CLS_SHAMAN_IDX;
        case 'l':
            return CLS_WARLOCK_IDX;
        case 'w':
            return CLS_WARRIOR_IDX;
        default:
            return -1;
    }
}
