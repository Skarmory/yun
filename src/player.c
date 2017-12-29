#include "player.h"

#include <stdlib.h>

struct Player* you;

void destroy_player(void)
{
    if(you != NULL)
    {
        free(you->cls);
        free(you->race);
        if(you->mon != NULL)
            free(you->mon);
        free(you); 
    }
}
