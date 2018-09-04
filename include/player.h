#ifndef PLAYER_H
#define PLAYER_H

#include "class.h"
#include "race.h"
#include "mon.h"
#include "stats.h"

/* Struct containing information about the player */
struct Player 
{
    char* name;
    char* faction;

    struct Class* cls;
    struct Race* race;

    struct Mon* mon;
};

/* Create a new player. There should only ever be one player */
void new_player(void);

/* Destroy the player */
void destroy_player(void);

/* Check if the player is dead. If so, print out death message and quit the game */
void player_chk_dead(void);

extern struct Player* you;

#endif
