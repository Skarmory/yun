#include "player_faction.h"

/**
 * Array of all factions
 */
char* factions[] =
{
    "none"
};

/**
 * Returns the name of the faction by the faction ID
 */
const char* faction_get_name_by_idx(int idx)
{
    if(idx >= 0 && idx < 4)
        return factions[idx];
    return "unknown";
}
