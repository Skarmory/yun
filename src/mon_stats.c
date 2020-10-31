#include "mon_stats.h"

#include "monster.h"

void mon_set_stat(struct Mon* mon, enum StatType stat, int amount)
{
    switch(stat)
    {
        case STAT_TYPE_HP:
            mon->stats.hp = amount;
            break;
        case STAT_TYPE_HP_MAX:
            mon->stats.hp_max = amount;
            break;
    }
}
