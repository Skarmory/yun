#include "player_race.h"

#include "mon_type.h"
#include "player_faction.h"

#include <stddef.h>

#define RACE(hk, noun, adj, home, lead, smask, fac) \
    {\
      hk, noun, adj, home, lead, smask, fac  \
    }\

#define STATUPS(str, agi, in, spi, sta)\
    str, agi, in, spi, sta\

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

/**
 * Gets the bitmask for a given race
 */
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

/*
 * Get race index by character (index corresponds to location in the races array)
 */
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

/*
 * Get race montype by character (index corresponds to location in the races array)
 */
struct MonType* get_race_montype(char race)
{
    switch(race)
    {
        case 'h':
            return &mon_type[MT_HUMAN] ;
        case 'd':
            return &mon_type[MT_DWARF];
        case 'e':
            return &mon_type[MT_NELF];
        case 'g':
            return &mon_type[MT_GNOME];
        case 'o':
            return &mon_type[MT_ORC];
        case 'f':
            return &mon_type[MT_FORSAKEN];
        case 't':
            return &mon_type[MT_TAUREN];
        case 'l':
            return &mon_type[MT_TROLL];
        default:
            return NULL;
    }
}
