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

typedef enum _AttackResult
{
    AR_HIT,
    AR_MISS,
    AR_DODGE,
    AR_PARRY,
    AR_BLOCK,
    AR_CRIT_BLOCK
} AttackResult;

/**
 * Check to see if attack hits
 *
 * Takes into account mon dodge/block etc not just hit
 */
static AttackResult _get_attack_result(struct Mon* attacker, struct Mon* defender)
{
    log_format_msg(DEBUG, ">>> %s(%d) -> %s(%d)", attacker->type->name, get_health(attacker), defender->type->name, get_health(defender));

    int str_diff = get_strength(attacker) - get_strength(defender);
    int agi_diff = get_agility(attacker) - get_agility(defender);

    log_format_msg(DEBUG, "%d + %d = %d", str_diff, agi_diff, str_diff + agi_diff);

    int roll = roll_d100();

    log_format_msg(DEBUG, "roll to hit: %d (need > %d)", roll, (5 - (str_diff + agi_diff) / 10));

    // Check for certain hit or miss
    if(roll < c_certain_miss_threshold)
        return AR_MISS;
    if(roll > c_certain_hit_threshold)
        return AR_HIT;

    // Check if hit
    if(roll < (5 - (str_diff + agi_diff) / 10))
        return AR_MISS;

    // Check for defender dodge
    if(dodge_check(defender))
        return AR_DODGE;

    // Check for defender parry
    if(parry_check(defender))
        return AR_PARRY;

    // Check for defender block
    if(mon_shield_wielding(defender) && block_check(defender))
    {
        // Check for crit block
        if(crit_block_check(defender))
            return AR_CRIT_BLOCK;

        return AR_BLOCK;
    }

    log_msg(DEBUG, "hit");
    log_msg(DEBUG, "<<<");

    return AR_HIT;
}

static int _attack_power_damage(struct Mon* attacker)
{
    return (get_attack_power(attacker) / c_attack_power_divisor) + 1;
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

    dmg += _attack_power_damage(attacker);

    return dmg;
}

static inline float _calc_armour_reduction(struct Mon* attacker, struct Mon* defender)
{
    return log_base(((equipment_armour_total(defender->equipment) - get_armour_pen(attacker)) / c_armour_value_divisor) + 1, c_armour_value_log_base);
}

static inline void _display_hit_text(struct Mon* attacker, struct Mon* defender, int damage)
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

static inline void _display_dodge_text(struct Mon* attacker, struct Mon* defender)
{
    if(mon_is_player(attacker))
    {
        display_fmsg_log("The %s dodged.", defender->type->name);
    }
    else if(mon_is_player(defender))
    {
        display_msg_log("You dodged.");
    }
    else
    {
        display_fmsg_log("The %s dodged the %s.", defender->type->name, attacker->type->name);
    }
}

static inline void _display_parry_text(struct Mon* attacker, struct Mon* defender)
{
    if(mon_is_player(attacker))
    {
        display_fmsg_log("The %s parried.", defender->type->name);
    }
    else if(mon_is_player(defender))
    {
        display_msg_log("You parried.");
    }
    else
    {
        display_fmsg_log("The %s parried the %s.", defender->type->name, attacker->type->name);
    }
}

static inline void _display_block_text(struct Mon* attacker, struct Mon* defender)
{
    if(mon_is_player(attacker))
    {
        display_fmsg_log("The %s blocked.", defender->type->name);
    }
    else if(mon_is_player(defender))
    {
        display_msg_log("You blocked.");
    }
    else
    {
        display_fmsg_log("The %s blocked the %s.", defender->type->name, attacker->type->name);
    }
}

static inline void _display_crit_block_text(struct Mon* attacker, struct Mon* defender)
{
    if(mon_is_player(attacker))
    {
        display_fmsg_log("The %s blocked. You stagger.", defender->type->name);
    }
    else if(mon_is_player(defender))
    {
        display_fmsg_log("You blocked. The %s staggers.", attacker->type->name);
    }
    else
    {
        display_fmsg_log("The %s blocked the %s. The %s staggers.", defender->type->name, attacker->type->name, attacker->type->name);
    }
}

bool do_attack_mon_mon(struct Mon* attacker, struct Mon* defender)
{
    AttackResult result = _get_attack_result(attacker, defender);

    int damage = 0;

    switch(result)
    {
        case AR_HIT:
            {
                damage = _attack_damage(attacker, defender);
                damage *= (1.0f - _calc_armour_reduction(attacker, defender));
                _display_hit_text(attacker, defender, damage);
            }
            break;

        case AR_MISS:
            {
                _display_miss_text(attacker, defender);
            }
            break;

        case AR_DODGE:
            {
                _display_dodge_text(attacker, defender);
            }
            break;

        case AR_PARRY:
            {
                _display_parry_text(attacker, defender);
            }
            break;

        case AR_BLOCK:
            {
                damage = _attack_damage(attacker, defender);
                damage -= defender->equipment->off_hand->objtype_ptr.armour->armour_value;
                damage *= (1.0f - _calc_armour_reduction(attacker, defender));
                _display_block_text(attacker, defender);
            }
            break;

        case AR_CRIT_BLOCK:
            {
                damage = _attack_damage(attacker, defender);
                damage -= defender->equipment->off_hand->objtype_ptr.armour->armour_value;
                damage *= (1.0f - _calc_armour_reduction(attacker, defender));
                _display_crit_block_text(attacker, defender);
            }
            break;
    }

    HP(defender) -= damage;
    if(mon_is_player(defender))
        player_chk_dead();
    else
        mon_chk_dead(defender);

    return true;
}

struct AttackMethod* attack_method_look_up_by_id(const char* id)
{
    for(int idx = 0; idx < g_attack_methods_count; ++idx)
    {
        if(strcmp(id, g_attack_methods[idx].id) == 0)
            return &g_attack_methods[idx];
    }

    return NULL;
}
