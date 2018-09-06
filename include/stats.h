#ifndef STATS_H
#define STATS_H

#include <stdbool.h>

#define STRENGTH     0
#define AGILITY      1
#define INTELLIGENCE 2
#define SPIRIT       3
#define STAMINA      4

#define PSTAT(s, t) you->mon->stats.s.t
#define MSTAT(m, s, t) m->stats.s.t

#define GET_PSTAT(s, t) PSTAT(s, t) > STAT_MAX ? STAT_MAX : PSTAT(s, t)
#define GET_MSTAT(m, s, t) MSTAT(m, s, t) > STAT_MAX ? STAT_MAX : MSTAT(m, s, t)

#define STAT_MAX      99
#define INV_STAT_MAX  (1.f/99.f)
#define PRIMARY_MOD   1.0f
#define SECONDARY_MOD 0.85f
#define TERTIARY_MOD  0.65f
#define OTHER_MOD     0.45f

struct Mon;

// Strength increases attack power, armour pen, and parry chanc.
// It is the affinity stat for warriors and paladins
struct Strength
{
    int base_strength;
    int strength;
    int attack_power;
    float armour_pen;
    float parry_chance;
};

// Agility increases attack power, physical crit, and dodge chance.
// It is the affinity stat for rogues and hunters
struct Agility
{
    int base_agility;
    int agility;
    int attack_power;
    float crit_chance;
    float dodge_chance;
};

// Intelligence increases mana pool, spellpower, spell crit, and spell pen.
// It is the affinity stat for mages, warlocks, and shamans.
struct Intelligence
{
    int base_intelligence;
    int intelligence;
    int max_mana;
    int mana;
    int spell_power;
    int spell_crit_chance;
    int spell_pen;
};

// Stamina increases health, block chance, critical block chance, and block amount.
// It is the only "primary stat" that no class has an affinity for. It is equally good for all classes.
struct Stamina
{
    int base_stamina;
    int stamina;
    int max_health;
    int health;
    float block_chance;
    float crit_block_chance;
    int block_amount;
};

// Spirit grants bonuses to mana and health regeneration, spellpower, and base resistance
// It is the affinity stat for priests
struct Spirit
{
    int base_spirit;
    int spirit;
    int mana_regen;
    int health_regen;
    int resist;
    int spell_power;
};

struct Stats
{
    struct Strength strength;
    struct Agility agility;
    struct Intelligence intelligence;
    struct Stamina stamina;
    struct Spirit spirit;

    short primary, secondary, tertiary;
};

void add_stat(struct Mon* mon, int stat, int amount, bool base);
void set_stat(struct Mon* mon, int stat, int amount);

#endif
