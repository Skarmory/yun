#ifndef YUN_ATTACK_H
#define YUN_ATTACK_H

#include <stdbool.h>

struct Mon;

struct AttackMethod
{
    char id[5];
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

struct AttackMethod* attack_method_look_up_by_id(const char* id);

bool do_attack_mon_mon(struct Mon* attacker, struct Mon* defender);

extern struct AttackMethod* g_attack_methods;
extern int g_attack_methods_count;

#endif
