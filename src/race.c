#include "race.h"

#define RACE(hk, noun, adj, home, lead, smask, fac, statups) \
    {\
      hk, noun, adj, home, lead, smask, fac, statups  \
    }\

#define STATUPS(str, agi, in, spi, sta)\
    str, agi, in, spi, sta\

struct Race races[] =
{
    RACE('h', "human", "human", "Stormwind", "Highlord Fordragon", RA_HUMAN, FA_ALLIANCE, STATUPS(1, 1, 1, 1, 1)),
    RACE('d', "dwarf", "dwarven", "Ironforge", "King Bronzebeard", RA_DWARF, FA_ALLIANCE, STATUPS(2, 0, 0, 1, 2)),
    RACE('e', "night elf", "night elven", "Darnassus", "High Priestess Whisperwind", RA_NELF, FA_ALLIANCE, STATUPS(1, 3, 1, 0, 0)),
    RACE('g', "gnome", "gnomish", "Ironforge", "High Tinker Mekkatorque", RA_GNOME, FA_ALLIANCE, STATUPS(0, 1, 3, 1, 0)),
    RACE('o', "orc", "orcish", "Orgrimmar", "Warchief Thrall", RA_ORC, FA_HORDE, STATUPS(3, 0, 0, 0, 2)),
    RACE('f', "forsaken", "forsaken", "The Undercity", "Banshee Queen Windrunner", RA_FORSAKEN, FA_HORDE, STATUPS(1, 1, 3, 0, 0)),
    RACE('t', "tauren", "tauren", "Thunder Bluff", "Chieftain Bloodhoof", RA_TAUREN, FA_HORDE, STATUPS(2, 0, 0, 0, 3)),
    RACE('l', "troll", "troll", "Orgrimmar", "Darkspear Leader Vol'jin", RA_TROLL, FA_HORDE, STATUPS(1, 2, 0, 2, 0))
};
