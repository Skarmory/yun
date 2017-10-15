#ifndef MON_H
#define MON_H

#include "montype.h"

struct Mon
{
    struct Mon* next; // linked list for mons on the level
    unsigned int x, y;
    struct MonType* type;
};

struct Mon* gen_mon(int mtype, int x, int y);
void kill_mon(struct Mon*);

#endif
