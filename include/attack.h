#ifndef attack_h
#define attack_h

#include <stdbool.h>
#include "mon.h"

/* Damage a monster with the player's weapon. Check if the monster died and print results */
bool do_player_attack_mon(struct Mon* defender);

/* Damage the player with a monster's weapon. Check if the player died and print results */
bool do_mon_attack_player(struct Mon* attacker);

#endif
