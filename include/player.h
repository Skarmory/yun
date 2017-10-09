#ifndef PLAYER_H
#define PLAYER_H

#include "class.h"
#include "race.h"
#include "mon.h"

struct Player {
    char* name;
    char* faction;

    struct Class* cls;
    struct Race* race;

    struct Mon* mon;
};

extern struct Player* you;

#endif
