#ifndef attack_h
#define attack_h

#include <stdbool.h>
#include "mon.h"

bool do_player_attack_mon(struct Mon* defender);
bool do_mon_attack_player(struct Mon* attacker);

#endif
