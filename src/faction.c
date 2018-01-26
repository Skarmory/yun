#include "faction.h"

char* factions[] =
{
    "none", "alliance", "horde", "scourge"
};

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

char* get_faction_name(int idx)
{
    if(idx >= 0 && idx < 4)
        return factions[idx];
    else
        return "unknown";
}
