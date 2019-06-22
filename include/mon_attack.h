#ifndef NAXX_ATTACK_H
#define NAXX_ATTACK_H

#include <stdbool.h>

struct Mon;

struct AttackMethod
{
    char name[32];
    char msg1[32];
    char msg2[32];
};

/**
 * Struct containing data for an attack
 */
struct Attack
{
    int num_dice;
    int sides_per_die;
    struct AttackMethod* method;
};

struct AttackMethod* attack_method_lookup_by_name(const char* name);

bool do_attack_mon_mon(struct Mon* attacker, struct Mon* defender);

extern struct AttackMethod* g_attack_methods;
extern int g_attack_methods_count;

#endif
