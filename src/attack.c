#include "attack.h"
#include "player.h"
#include "log.h"
#include "util.h"
#include "attack_data.h"
#include "message.h"
#include <stdio.h>

/**
 * Do the actual damage calculation and reduce HP here
 *
 * "Roll" damage dice and minus HP from the defender
 */
void _do_attack(struct Mon* attacker, struct Mon* defender, int* dmg, struct Weapon* weapon)
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
    struct Weapon* weapon = mon_get_weapon(you->mon);
    int dmg;

    _do_attack(you->mon, defender, &dmg, weapon);

    display_format_msg("You hit the %s for %d (%dd%d) with your %s.", defender->type->name, dmg, weapon->attk->num_dice, weapon->attk->sides_per_die, weapon->name);

    mon_chk_dead(defender);

    return true;
}

/**
 * Damage the player with a monster's weapon. Check if the player died and print results
 */
bool do_mon_attack_player(struct Mon* attacker)
{
    struct Weapon* weapon = mon_get_weapon(attacker);
    int dmg;

   _do_attack(attacker, you->mon, &dmg, weapon);

    display_format_msg("The %s hit you for %d (%dd%d) with its %s.", attacker->type->name, dmg, weapon->attk->num_dice, weapon->attk->sides_per_die, weapon->name);

    player_chk_dead();

    return true;
}
