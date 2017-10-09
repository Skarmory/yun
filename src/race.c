#include "race.h"

#define RACE(hk, noun, adj, home, lead, smask, fac) \
    {\
      hk, noun, adj, home, lead, smask, fac  \
    }\

struct Race races[] =
{
    RACE('h', "human", "human", "Stormwind", "Highlord Fordragon", RA_HUMAN, FA_ALLIANCE),
    RACE('d', "dwarf", "dwarven", "Ironforge", "King Bronzebeard", RA_DWARF, FA_ALLIANCE),
    RACE('e', "night elf", "night elven", "Darnassus", "High Priestess Whisperwind", RA_NELF, FA_ALLIANCE),
    RACE('g', "gnome", "gnomish", "Ironforge", "High Tinker Mekkatorque", RA_GNOME, FA_ALLIANCE),
    RACE('o', "orc", "orcish", "Orgrimmar", "Warchief Thrall", RA_ORC, FA_HORDE),
    RACE('f', "forsaken", "forsaken", "The Undercity", "Banshee Queen Windrunner", RA_FORSAKEN, FA_HORDE),
    RACE('t', "tauren", "tauren", "Thunder Bluff", "Chieftain Bloodhoof", RA_TAUREN, FA_HORDE),
    RACE('l', "troll", "troll", "Orgrimmar", "Darkspear Leader Vol'jin", RA_TROLL, FA_HORDE)
};
