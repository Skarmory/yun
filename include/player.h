#ifndef PLAYER_H
#define PLAYER_H

#include "class.h"
#include "race.h"

struct Player {
    char* name;
    char* faction;

    struct Class* cls;
    struct Race* race;
};

extern struct Player* you;

#endif
