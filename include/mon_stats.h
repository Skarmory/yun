#ifndef YUN_STATS_H
#define YUN_STATS_H

// TODO: Pending stat system redesign

#include <stdbool.h>

struct Mon;

enum StatType
{
    STAT_TYPE_HP,
    STAT_TYPE_HP_MAX
};

/**
 * Struct containing the stats data for a mon
 */
struct Stats
{
    int hp;
    int hp_max;
};

void mon_set_stat(struct Mon* mon, enum StatType stat, int amount);

#endif
