#include "faction.h"

/**
 * Array of all factions
 */
char* factions[] =
{
    "none", "alliance", "horde", "scourge"
};

/**
 * Returns the ID of the faction by the race of the creature
 */
int get_faction_by_race(char race)
{
    switch(race)
    {
        case 'h':
        case 'd':
        case 'e':
        case 'g':
            return FA_ALLIANCE;
        case 'o':
        case 'f':
        case 't':
        case 'l':
            return FA_HORDE;
        default:
            return -1;
    }
}

/**
 * Returns the name of the faction by the faction ID
 */
char* get_faction_name(int idx)
{
    if(idx >= 0 && idx < 4)
        return factions[idx];
    return "unknown";
}
