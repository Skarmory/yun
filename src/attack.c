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
    hit = 0,
    miss = 1,
    dodge = 2,
    parry = 3,
    block = 4,
    crit_block = 5
} attack_result;

/**
 * Check to see if attack hits
 *
 * Takes into account mon dodge/block etc not just hit
 */
attack_result _get_attack_result(struct Mon* attacker, struct Mon* defender)
{
    log_format_msg(">>> %s -> %s", DEBUG, attacker->type->name, defender->type->name);

    int str_diff = MSTAT(attacker, strength, strength) - MSTAT(defender, strength, strength);
    int agi_diff = MSTAT(attacker, agility, agility) - MSTAT(defender, agility, agility);

    log_format_msg("%d + %d = %d", DEBUG, str_diff, agi_diff, str_diff + agi_diff);

    int roll = roll_d100();

    log_format_msg("roll to hit: %d (need > %d)", DEBUG, roll, (5 - (str_diff + agi_diff) / 10));

    // Check for guaranteed hit or miss
    if(roll < 5)
        return miss;
    if(roll > 95)
        return hit;

    // Check if hit
    if(roll < (5 - (str_diff + agi_diff) / 10))
        return miss;

    // Check for defender dodge
    if(dodge_check(defender))
        return dodge;

    // Check for defender parry
    if(parry_check(defender))
        return parry;

    // Check for defender block
    if(block_check(defender))
    {
        // Check for crit block
        roll = roll_d100();
        log_format_msg("roll to crit block: %d (need > %d)", DEBUG, roll, 100 - (int)(MSTAT(defender, stamina, crit_block_chance) * 100));
        if(roll > 100 - (int)(MSTAT(defender, stamina, crit_block_chance) * 100))
            return crit_block;

        return block;
    }

    log_msg("hit", DEBUG);
    log_msg("<<<", DEBUG);

    return hit;
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
        case hit:
            _do_attack(you->mon, defender, &dmg, weapon);
            display_format_msg("You hit the %s for %d (%dd%d) with your %s.", defender->type->name, dmg, weapon->attk->num_dice, weapon->attk->sides_per_die, weapon->obj->name);
            mon_chk_dead(defender);
            break;
        case miss:
            display_msg("You missed.");
            break;
        case dodge:
            display_format_msg("The %s dodged.", defender->type->name);
            break;
        case parry:
            display_format_msg("The %s parried.", defender->type->name);
            break;
        case block:
            display_format_msg("The %s blocked.", defender->type->name);
            break;
        case crit_block:
            display_format_msg("The %s blocked.", defender->type->name);
            break;
        default:
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
        case hit:
            _do_attack(attacker, you->mon, &dmg, weapon);
            display_format_msg("The %s hit you for %d (%dd%d) with its %s.", attacker->type->name, dmg, weapon->attk->num_dice, weapon->attk->sides_per_die, weapon->obj->name);
            player_chk_dead();
            break;
        case miss:
            display_msg("You missed.");
            break;
        case dodge:
            display_msg("You dodged.");
            break;
        case parry:
            display_msg("You parried.");
            break;
        case block:
            // TODO: Determine blocked damage
            display_msg("You blocked.");
            break;
        case crit_block:
            // TODO: Determine blocked damage
            display_msg("You blocked.");
            break;
        default:
            break;
    }

    return true;
}
