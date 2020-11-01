#include "mon_attack.h"

#include "log.h"
#include "message.h"
#include "monster.h"
#include "mon_equip.h"
#include "mon_stats.h"
#include "mon_type.h"
#include "object.h"
#include "obj_armour.h"
#include "obj_weapon.h"
#include "player.h"
#include "util.h"

#include <stdio.h>
#include <string.h>

struct AttackMethod* g_attack_methods = NULL;
int g_attack_methods_count = 0;

const int c_certain_miss_threshold = 5;
const int c_certain_hit_threshold = 95;
const int c_attack_power_divisor  = 12;
const int c_armour_value_divisor  = 50;
const int c_armour_value_log_base = 5;

enum AttackResult
{
    AR_HIT,
    AR_MISS
};

/**
 * Check to see if attack hits
 *
 * Takes into account mon dodge/block etc not just hit
 */
static enum AttackResult _get_attack_result(struct Mon* attacker, struct Mon* defender)
{
    log_format_msg(LOG_DEBUG, ">>> %s(%d) -> %s(%d)", attacker->type->name, attacker->stats.hp, defender->type->name, defender->stats.hp);

    int roll = roll_d100();

    log_format_msg(LOG_DEBUG, "roll to hit: %d", roll);

    // Check for certain hit or miss
    if(roll < c_certain_miss_threshold)
    {
        log_msg(LOG_DEBUG, "miss");
        log_msg(LOG_DEBUG, "<<<");
        return AR_MISS;
    }
    else
    {
        log_msg(LOG_DEBUG, "hit");
        log_msg(LOG_DEBUG, "<<<");
        return AR_HIT;
    }
}

/**
 * Do the actual damage calculation and reduce HP here
 *
 * "Roll" damage dice and minus HP from the defender
 */
static int _attack_damage(struct Mon* attacker, struct Mon* defender)
{
    int dmg = 0;
    struct Attack* attks = NULL;

    const struct Weapon* weapon = mon_get_weapon(attacker);
    if(weapon)
    {
        attks = weapon->base->attk;
    }
    else
    {
        attks = defender->type->base_weapon->attk;
    }

    for(int dice = 0; dice < attks->num_dice; dice++)
    {
        dmg += random_int(1, attks->sides_per_die);
    }

    return dmg;
}

static inline void _display_hit_text(struct Mon* attacker, struct Mon* defender)
{
    const struct Weapon* attacker_weapon = mon_get_weapon(attacker);
    const struct Attack* attacks = NULL;

    if(attacker_weapon)
    {
        attacks = attacker_weapon->base->attk;
    }
    else
    {
        attacks = attacker->type->base_weapon->attk;
    }

    char method_expanded[256];
    char attacker_expanded[64];
    char defender_expanded[64];

    if(mon_is_player(attacker))
    {
        snprintf(method_expanded, sizeof(method_expanded), "%s.", attacks->method->msg1);
        snprintf(attacker_expanded, sizeof(attacker_expanded), "%s", "You");
        snprintf(defender_expanded, sizeof(defender_expanded), "the %s", defender->type->name);
    }
    else if(mon_is_player(defender))
    {
        snprintf(method_expanded, sizeof(method_expanded), "%s.", attacks->method->msg2);
        snprintf(attacker_expanded, sizeof(attacker_expanded), "The %s", attacker->type->name);
        snprintf(defender_expanded, sizeof(defender_expanded), "%s", "you");
    }
    else
    {
        snprintf(method_expanded, sizeof(method_expanded), "%s.", attacks->method->msg2);
        snprintf(attacker_expanded, sizeof(attacker_expanded), "The %s", attacker->type->name);
        snprintf(defender_expanded, sizeof(defender_expanded), "the %s", defender->type->name);
    }

    display_fmsg_log(method_expanded, attacker_expanded, defender_expanded);
}

static inline void _display_miss_text(struct Mon* attacker, struct Mon* defender)
{
    if(mon_is_player(attacker))
    {
        display_msg_log("You missed.");
    }
    else if(mon_is_player(defender))
    {
        display_fmsg_log("The %s missed.", attacker->type->name);
    }
    else
    {
        display_fmsg_log("The %s missed the %s.", attacker->type->name, defender->type->name);
    }
}

bool do_attack_mon_mon(struct Mon* attacker, struct Mon* defender)
{
    enum AttackResult result = _get_attack_result(attacker, defender);

    int damage = 0;

    switch(result)
    {
        case AR_HIT:
        {
            damage = _attack_damage(attacker, defender);
            _display_hit_text(attacker, defender);
            break;
        }

        case AR_MISS:
        {
            _display_miss_text(attacker, defender);
            break;
        }
    }

    defender->stats.hp -= damage;

    if(mon_is_player(defender))
    {
        player_chk_dead();
    }
    else
    {
        mon_chk_dead(defender);
    }

    return true;
}

struct AttackMethod* attack_method_look_up_by_id(const char* id)
{
    for(int idx = 0; idx < g_attack_methods_count; ++idx)
    {
        if(strcmp(id, g_attack_methods[idx].id) == 0)
        {
            return &g_attack_methods[idx];
        }
    }

    return NULL;
}
