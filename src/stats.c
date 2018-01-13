#include "stats.h"
#include "mon.h"
#include "log.h"
#include <stdio.h>

void _add_strength(struct Mon* mon, int amount, bool base)
{
    if(base)
        MSTAT(mon, strength, base_strength) += amount;
    MSTAT(mon, strength, strength) += amount;
}

void _add_agility(struct Mon* mon, int amount, bool base)
{
    if(base)
        MSTAT(mon, agility, base_agility) += amount;
    MSTAT(mon, agility, agility) += amount;
}

void _add_intelligence(struct Mon* mon, int amount, bool base)
{
    if(base)
        MSTAT(mon, intelligence, base_intelligence) += amount;
    MSTAT(mon, intelligence, intelligence) += amount;
}

void _add_spirit(struct Mon* mon, int amount, bool base)
{
    if(base)
        MSTAT(mon, spirit, base_spirit) += amount;
    MSTAT(mon, spirit, spirit) += amount;
}

void _add_stamina(struct Mon* mon, int amount, bool base)
{
    if(base)
        MSTAT(mon, stamina, base_stamina) += amount;
    MSTAT(mon, stamina, stamina) += amount;
}

void add_stat(struct Mon* mon, int stat, int amount, bool base)
{
    switch(stat)
    {
        case STRENGTH:
            _add_strength(mon, amount, base);
            break;
        case AGILITY:
            _add_agility(mon, amount, base);
            break;
        case INTELLIGENCE:
            _add_intelligence(mon, amount, base);
            break;
        case SPIRIT:
            _add_spirit(mon, amount, base);
            break;
        case STAMINA:
            _add_stamina(mon, amount, base);
            break;
    }
}
