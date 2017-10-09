#include "mon.h"
#include <stdlib.h>

struct Mon* gen_mon(struct MonType* montype, int x, int y)
{
    struct Mon* mon = (struct Mon*) malloc(sizeof(struct Mon));

    mon->next = NULL;
    mon->type = montype;
    mon->x = x;
    mon->y = y;

    return mon;
}

void kill_mon(struct Mon* mon)
{
    free(mon);
}
