#ifndef MON_H
#define MON_H

#include "montype.h"

struct Mon
{
    struct Mon* next; // linked list for mons on the level
    unsigned int x, y;
    struct MonType* type;
};

const struct Mon MON_NULL = { 0 };

struct Mon* gen_mon(struct MonType* mon, int x, int y);
void kill_mon(struct Mon*);

#endif
