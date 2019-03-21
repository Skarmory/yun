#ifndef NAXX_ATTACK_H
#define NAXX_ATTACK_H

#include <stdbool.h>

struct Mon;

/**
 * Struct containing data for an attack
 */
struct Attack
{
    int num_dice;
    int sides_per_die;
};

bool do_player_attack_mon(struct Mon* defender);
bool do_mon_attack_player(struct Mon* attacker);

#endif
