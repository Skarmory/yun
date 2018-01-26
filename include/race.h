#ifndef RACE_H
#define RACE_H

#include "faction.h"

// Race masks 
#define RA_HUMAN    0x00000001
#define RA_DWARF    0x00000002
#define RA_NELF     0x00000004
#define RA_GNOME    0x00000008
#define RA_ORC      0x00000010
#define RA_FORSAKEN 0x00000020
#define RA_TAUREN   0x00000040
#define RA_TROLL    0x00000080

#define RA_HUMAN_IDX    0
#define RA_DWARF_IDX    1
#define RA_NELF_IDX     2
#define RA_GNOME_IDX    3
#define RA_ORC_IDX      4
#define RA_FORSAKEN_IDX 5
#define RA_TAUREN_IDX   6
#define RA_TROLL_IDX    7

struct Race
{
    char hotkey;
    char* noun;
    char* adj;
    char* home;
    char* leader;
    short selfmask;
    short faction;
    short strength_up, agility_up, intelligence_up, spirit_up, stamina_up;
};

extern struct Race races[];

int get_race_mask(char race);
int get_race_idx(char race);

#endif
