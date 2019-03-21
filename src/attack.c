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

typedef enum attack_result
{
    AR_HIT,
    AR_MISS,
    AR_DODGE,
    AR_PARRY,
    AR_BLOCK,
    AR_CRIT_BLOCK
} attack_result;

/**
 * Check to see if attack hits
 *
 * Takes into account mon dodge/block etc not just hit
 */
static attack_result _get_attack_result(struct Mon* attacker, struct Mon* defender)
{
    log_format_msg(DEBUG, ">>> %s -> %s", attacker->type->name, defender->type->name);

    int str_diff = MSTAT(attacker, strength, strength) - MSTAT(defender, strength, strength);
    int agi_diff = MSTAT(attacker, agility, agility) - MSTAT(defender, agility, agility);

    log_format_msg(DEBUG, "%d + %d = %d", str_diff, agi_diff, str_diff + agi_diff);

    int roll = roll_d100();

    log_format_msg(DEBUG, "roll to hit: %d (need > %d)", roll, (5 - (str_diff + agi_diff) / 10));

    // Check for guaranteed hit or miss
    if(roll < 5)
        return AR_MISS;
    if(roll > 95)
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
        roll = roll_d100();
        log_format_msg(DEBUG, "roll to crit block: %d (need > %d)", roll, 100 - (int)(MSTAT(defender, stamina, crit_block_chance) * 100));
        if(roll > 100 - (int)(MSTAT(defender, stamina, crit_block_chance) * 100))
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
void _do_attack(struct Mon* attacker, struct Mon* defender, int* dmg, const struct Weapon* weapon)
{
    *dmg = 0;

    for(int dice = 0; dice < weapon->attk->num_dice; dice++)
    {
        *dmg += random_int(1, weapon->attk->sides_per_die);
    }

    HP(defender) -= *dmg;
}

/**
 * Damage a monster with the player's weapon. Check if the monster died and print results
 */
bool do_player_attack_mon(struct Mon* defender)
{
    const struct Weapon* weapon = mon_get_weapon(you->mon);
    int dmg;

    switch(_get_attack_result(you->mon, defender))
    {
        case AR_HIT:
            _do_attack(you->mon, defender, &dmg, weapon);
            display_fmsg_log("You hit the %s for %d (%dd%d) with your %s.", defender->type->name, dmg, weapon->attk->num_dice, weapon->attk->sides_per_die, weapon->obj->name);
            mon_chk_dead(defender);
            break;
        case AR_MISS:
            display_msg_log("You missed.");
            break;
        case AR_DODGE:
            display_fmsg_log("The %s dodged.", defender->type->name);
            break;
        case AR_PARRY:
            display_fmsg_log("The %s parried.", defender->type->name);
            break;
        case AR_BLOCK:
            display_fmsg_log("The %s blocked.", defender->type->name);
            break;
        case AR_CRIT_BLOCK:
            display_fmsg_log("The %s blocked. You stagger.", defender->type->name);
            break;
    }

    return true;
}

/**
 * Damage the player with a monster's weapon. Check if the player died and print results
 */
bool do_mon_attack_player(struct Mon* attacker)
{
    const struct Weapon* weapon = mon_get_weapon(attacker);
    int dmg;

    switch(_get_attack_result(attacker, you->mon))
    {
        case AR_HIT:
            _do_attack(attacker, you->mon, &dmg, weapon);
            display_fmsg_log("The %s hit you for %d (%dd%d) with its %s.", attacker->type->name, dmg, weapon->attk->num_dice, weapon->attk->sides_per_die, weapon->obj->name);
            player_chk_dead();
            break;
        case AR_MISS:
            display_msg_log("You missed.");
            break;
        case AR_DODGE:
            display_msg_log("You dodged.");
            break;
        case AR_PARRY:
            display_msg_log("You parried.");
            break;
        case AR_BLOCK:
            // TODO: Determine blocked damage
            display_msg_log("You blocked.");
            break;
        case AR_CRIT_BLOCK:
            // TODO: Determine blocked damage
            display_fmsg_log("You blocked. The %s staggers.", attacker->type->name);
            break;
    }

    return true;
}
