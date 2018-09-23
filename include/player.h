#ifndef PLAYER_H
#define PLAYER_H

#include "class.h"
#include "race.h"
#include "mon.h"
#include "stats.h"

/**
 * Struct containing information about the player
 */
struct Player
{
    char* name;
    char* faction;

    struct Class* cls;
    struct Race* race;

    struct Mon* mon;
};

void new_player(void);
void destroy_player(void);
void player_chk_dead(void);

extern struct Player* you;

#endif
