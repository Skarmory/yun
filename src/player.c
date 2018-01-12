#include "player.h"

#include <stdlib.h>

struct Player* you;

void new_player(void)
{
    you = (struct Player*)malloc(sizeof(struct Player));
    you->name = NULL;
    you->faction = NULL;
    you->cls = NULL;
    you->race = NULL;
    you->mon = NULL;
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
