#ifndef CLASS_H
#define CLASS_H

#include "race.h"

#define CLS_HUNTER  0x00000001
#define CLS_MAGE    0x00000002
#define CLS_ROGUE   0x00000004
#define CLS_PALADIN 0x00000008
#define CLS_PRIEST  0x00000010
#define CLS_SHAMAN  0x00000020
#define CLS_WARLOCK 0x00000040
#define CLS_WARRIOR 0x00000080

struct Class 
{
    char* name;
    char hotkey;

    short allow_races;
    short selfmask;
};

const struct Class classes[] = 
{
    {
        "Hunter", 'h',
        RA_DWARF | RA_NELF | RA_ORC | RA_TAUREN | RA_TROLL,
        CLS_HUNTER
    },
    {
        "Mage", 'm',
        RA_HUMAN | RA_DWARF | RA_GNOME | RA_FORSAKEN | RA_TROLL,
        CLS_MAGE
    },
    {
        "Rogue",'r',
        RA_HUMAN | RA_DWARF | RA_NELF | RA_GNOME | RA_ORC | RA_FORSAKEN | RA_TROLL,
        CLS_ROGUE
    },
    {
        "Paladin", 'p',
        RA_HUMAN | RA_DWARF,
        CLS_PALADIN
    },
    {
        "Priest", 'i',
        RA_HUMAN | RA_DWARF | RA_NELF | RA_FORSAKEN | RA_TROLL,
        CLS_PRIEST
    },
    {
        "Shaman", 's',
        RA_ORC | RA_TAUREN | RA_TROLL,
        CLS_SHAMAN
    },
    {
        "Warlock", 'l',
        RA_HUMAN | RA_GNOME | RA_ORC | RA_FORSAKEN,
        CLS_WARLOCK
    },
    {
        "Warrior", 'w',
        RA_HUMAN | RA_DWARF | RA_NELF | RA_GNOME | RA_ORC | RA_FORSAKEN | RA_TAUREN | RA_TROLL,
        CLS_WARRIOR
    }
};

#endif
