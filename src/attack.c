#include "attack.h"
#include "util.h"
#include "attack_data.h"

#include <stdio.h>

bool do_attack(struct Mon* attacker, struct Mon* defender)
{
    struct Weapon* weapon = get_weapon(attacker);
    int dmg = 0;

    for(int dice = 0; dice < weapon->attk->num_dice; dice++)
    {
        dmg += random_int(1, weapon->attk->sides_per_die);
    }

    defender->hp -= dmg;

    chk_dead(defender);

    return true;
}
