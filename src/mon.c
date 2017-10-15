#include "mon.h"
#include <stdlib.h>

struct Mon* gen_mon(int mtype, int x, int y)
{
    struct Mon* mon = (struct Mon*) malloc(sizeof(struct Mon));

    mon->next = NULL;
    mon->type = &mon_type[mtype];
    mon->x = x;
    mon->y = y;

    return mon;
}

void kill_mon(struct Mon* mon)
{
    free(mon);
}
