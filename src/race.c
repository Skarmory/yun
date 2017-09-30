#include "race.h"

const struct Race races[] =
{
    {
        'h', "human", "human", "Stormwind", "Highlord Fordragon",
        RA_HUMAN,
        FA_ALLIANCE,
    },
    {
        'd', "dwarf", "dwarven", "Ironforge", "King Bronzebeard",
        RA_DWARF,
        FA_ALLIANCE,
    },
    {
        'e', "night elf", "night elven", "Darnassus", "High Priestess Whisperwind",
        RA_NELF,
        FA_ALLIANCE,
    },
    {
        'g', "gnome", "gnomish", "Ironforge", "High Tinker Mekkatorque",
        RA_GNOME,
        FA_ALLIANCE,
    },
    {
        'o', "orc", "orcish", "Orgrimmar", "Warchief Thrall",
        RA_ORC,
        FA_HORDE,
    },
    {
        'f', "forsaken", "forsaken", "The Undercity", "Banshee Queen Windrunner",
        RA_FORSAKEN,
        FA_HORDE,
    },
    {
        't', "tauren", "tauren", "Thunder Bluff", "Chieftain Bloodhoof",
        RA_TAUREN,
        FA_HORDE,
    },
    {
        'l', "troll", "troll", "Orgrimmar", "Darkspear Leader Vol'jin",
        RA_TROLL,
        FA_HORDE,
    }
};
