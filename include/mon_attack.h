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

bool do_attack_mon_mon(struct Mon* attacker, struct Mon* defender);

#endif
