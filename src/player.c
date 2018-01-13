#include "player.h"

#include <stdlib.h>

struct Player* you;

void new_player(void)
{
    you = (struct Player*)malloc(sizeof(struct Player));
    you->mon = new_mon(MT_PLAYER, 0, 0);
    you->name = NULL;
    you->faction = NULL;
    you->cls = NULL;
    you->race = NULL;
}

void destroy_player(void)
{
    if(you != NULL)
    {
        you->faction = NULL;
        free(you->cls);
        free(you->race);
        if(you->mon != NULL)
            free(you->mon);
        free(you); 
    }
}
