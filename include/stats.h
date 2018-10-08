#ifndef STATS_H
#define STATS_H

#include <stdbool.h>

struct Mon;

#define STRENGTH     0
#define AGILITY      1
#define INTELLIGENCE 2
#define SPIRIT       3
#define STAMINA      4

#define PSTAT(s, t) (you->mon->stats.s.t)
#define MSTAT(m, s, t) (m->stats.s.t)

#define GET_PSTAT(s, t) (PSTAT(s, t) > STAT_MAX ? STAT_MAX : PSTAT(s, t))
#define GET_MSTAT(m, s, t) (MSTAT(m, s, t) > STAT_MAX ? STAT_MAX : MSTAT(m, s, t))

#define STAT_MAX      99
#define INV_STAT_MAX  (1.f/99.f)

#define DODGE_MOD 4
#define PARRY_MOD 3

/**
 * Strength increases attack power, armour pen, and parry chance.
 */
struct Strength
{
    int base_strength;
    int strength;
    int attack_power;
    float armour_pen;
    float parry_chance;
    float scale;
};

/**
 * Agility increases attack power, physical crit, and dodge chance
 */
struct Agility
{
    int base_agility;
    int agility;
    int attack_power;
    float crit_chance;
    float dodge_chance;
    float scale;
};

/**
 * Intelligence increases mana pool, spellpower, spell crit, and spell pen.
 */
struct Intelligence
{
    int base_intelligence;
    int intelligence;
    int max_mana;
    int mana;
    int spell_power;
    int spell_crit_chance;
    int spell_pen;
    float scale;
};

/**
 * Stamina increases health, block chance, critical block chance, and block amount.
 */
struct Stamina
{
    int base_stamina;
    int stamina;
    int max_health;
    int health;
    float block_chance;
    float crit_block_chance;
    int block_amount;
    float scale;
};

/**
 * Spirit grants bonuses to mana and health regeneration, spellpower, and base resistance
 */
struct Spirit
{
    int base_spirit;
    int spirit;
    int mana_regen;
    int health_regen;
    int resist;
    int spell_power;
    float scale;
};

/**
 * Struct containing the stats data for a mon
 */
struct Stats
{
    struct Strength strength;
    struct Agility agility;
    struct Intelligence intelligence;
    struct Stamina stamina;
    struct Spirit spirit;
};

void add_stat(struct Mon* mon, int stat, int amount, bool base);
void set_stat(struct Mon* mon, int stat, int amount);

bool stat_check(struct Mon* mon, int stat);
bool dodge_check(struct Mon* mon);
bool parry_check(struct Mon* mon);

#endif
