#ifndef MONTYPE_H
#define MONTYPE_H

#define MT_GHOUL 0
#define MT_PLAYER 1

struct MonType
{
    char* name; // Name of this type of mon
    char sym; // ascii symbol of this mon
    signed char fg; // foreground colour
};

extern struct MonType mon_type[];

#endif
