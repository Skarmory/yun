#ifndef PLAYER_H
#define PLAYER_H

#include "class.h"
#include "race.h"
#include "mon.h"
#include "stats.h"

struct Player 
{
    char* name;
    char* faction;

    struct Class* cls;
    struct Race* race;

    struct Mon* mon;
    struct Stats* stats; 
};

void new_player(void);
void destroy_player(void);

extern struct Player* you;

#endif
