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

struct Race
{
    char hotkey;
    char* noun;
    char* adj;
    char* home;
    char* leader;
    short selfmask;
    short faction;
};

extern struct Race races[];

#endif
