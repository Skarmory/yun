#include "attack.h"
#include "log.h"
#include "util.h"
#include "attack_data.h"
#include "message.h"
#include <stdio.h>

bool do_attack(struct Mon* attacker, struct Mon* defender)
{
    struct Weapon* weapon = get_weapon(attacker);
    int dmg = 0;

    for(int dice = 0; dice < weapon->attk->num_dice; dice++)
    {
        dmg += random_int(1, weapon->attk->sides_per_die);
    }

    HP(defender) -= dmg;

    char buf[256];
    sprintf(buf, "You hit the %s for %d (%dd%d) with your %s.", defender->type->name, dmg, weapon->attk->num_dice, weapon->attk->sides_per_die, weapon->name);
    display_msg(buf);

    chk_dead(defender);

    return true;
}
