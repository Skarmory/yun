#include "attack.h"

#include "weapon.h"
#include "log.h"
#include "message.h"
#include "mon.h"
#include "montype.h"
#include "object.h"
#include "player.h"
#include "stats.h"
#include "util.h"

#include <stdio.h>

const int c_certain_miss_threshold = 5;
const int c_certain_hit_threshold = 95;

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
    if(block_check(defender))
    {
        // Check for crit block
        float rollf = roll_d100f();
        log_format_msg(DEBUG, "roll to crit block: %5.2f (need > %5.2f)", rollf * 100.0f, 100.0f - (get_crit_block(defender) * 100.0f));

        //if(roll > 100.0f - (MSTAT(defender, stamina, crit_block_chance) * 100.0f))
        if(rollf > get_crit_block(defender))
            return AR_CRIT_BLOCK;

        return AR_BLOCK;
    }

    log_msg(DEBUG, "hit");
    log_msg(DEBUG, "<<<");

    return AR_HIT;
}

/**
 * Do the actual damage calculation and reduce HP here
 *
 * "Roll" damage dice and minus HP from the defender
 */
static int _do_attack(struct Mon* attacker, struct Mon* defender)
{
    int dmg = 0;
    const struct Weapon* weapon = mon_get_weapon(you->mon);

    for(int dice = 0; dice < weapon->attk->num_dice; dice++)
    {
        dmg += random_int(1, weapon->attk->sides_per_die);
    }

    return dmg;
}

static inline void _display_hit_text(struct Mon* attacker, struct Mon* defender, int damage)
{
    const struct Weapon* attacker_weapon = mon_get_weapon(attacker);

    if(mon_is_player(attacker))
    {
        display_fmsg_log("You hit the %s for %d (%dd%d) with your %s.", defender->type->name, damage, attacker_weapon->attk->num_dice, attacker_weapon->attk->sides_per_die, attacker_weapon->obj->name);
    }
    else if(mon_is_player(defender))
    {
        display_fmsg_log("The %s hit you for %d (%dd%d) with its %s.", attacker->type->name, damage, attacker_weapon->attk->num_dice, attacker_weapon->attk->sides_per_die, attacker_weapon->obj->name);
    }
    else
    {
        display_fmsg_log("The %s hit the %s for %d (%dd%d) with its %s.", attacker->type->name, defender->type->name, damage, attacker_weapon->attk->num_dice, attacker_weapon->attk->sides_per_die, attacker_weapon->obj->name);
        mon_chk_dead(defender);
    }
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
    if(mon_is_player(defender))
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
    if(mon_is_player(defender))
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
                damage = _do_attack(attacker, defender);
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
                _display_block_text(attacker, defender);
            }
            break;

        case AR_CRIT_BLOCK:
            {
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
