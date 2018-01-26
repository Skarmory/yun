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

// Gets the bitmask for a given race
int get_race_mask(char race)
{
    switch(race)
    {
        case 'h':
            return RA_HUMAN;
        case 'd':
            return RA_DWARF;
        case 'e':
            return RA_NELF;
        case 'g':
            return RA_GNOME;
        case 'o':
            return RA_ORC;
        case 'f':
            return RA_FORSAKEN;
        case 't':
            return RA_TAUREN;
        case 'l':
            return RA_TROLL;
        default:
            return -1;
    }
}

// Get race index by character (index corresponds to location in the races array)
int get_race_idx(char race)
{
    switch(race)
    {
        case 'h':
            return RA_HUMAN_IDX;
        case 'd':
            return RA_DWARF_IDX;
        case 'e':
            return RA_NELF_IDX;
        case 'g':
            return RA_GNOME_IDX;
        case 'o':
            return RA_ORC_IDX;
        case 'f':
            return RA_FORSAKEN_IDX;
        case 't':
            return RA_TAUREN_IDX;
        case 'l':
            return RA_TROLL_IDX;
        default:
            return -1;
    }
}
