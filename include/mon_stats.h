#ifndef NAXX_STATS_H
#define NAXX_STATS_H

#include <stdbool.h>

struct Mon;

enum Stat
{
    STAT_STRENGTH,
    STAT_AGILITY,
    STAT_INTELLIGENCE,
    STAT_SPIRIT,
    STAT_STAMINA
};

#define STAT_MAX      99
#define INV_STAT_MAX  (1.f/99.f)

#define PSTAT(s, t)         (you->mon->stats.s.t)
#define MSTAT(m, s, t)      (m->stats.s.t)

#define GET_PSTAT(s, t)     (PSTAT(s, t) > STAT_MAX ? STAT_MAX : PSTAT(s, t))
#define GET_MSTAT(m, s, t)  (MSTAT(m, s, t) > STAT_MAX ? STAT_MAX : MSTAT(m, s, t))

#define get_strength(m)              (GET_MSTAT(m, strength, strength))
#define get_armour_pen(m)            (MSTAT(m, strength, armour_pen))
#define get_parry(m)                 (MSTAT(m, strength, parry_chance))
#define get_strength_attack_power(m) (MSTAT(m, strength, attack_power))

#define get_agility(m)              (GET_MSTAT(m, agility, agility))
#define get_crit(m)                 (MSTAT(m, agility, crit_chance))
#define get_dodge(m)                (MSTAT(m, agility, dodge_chance))
#define get_agility_attack_power(m) (MSTAT(m, agility, attack_power))

#define get_intelligence(m)             (GET_MSTAT(m, intelligence, intelligence))
#define get_mana(m)                     (MSTAT(m, intelligence, mana))
#define get_max_mana(m)                 (MSTAT(m, intelligence, max_mana))
#define get_intelligence_spell_power(m) (MSTAT(m, intelligence, spell_power))
#define get_spell_crit(m)               (MSTAT(m, intelligence, spell_crit_chance))
#define get_spell_pen(m)                (MSTAT(m, intelligence, spell_pen))

#define get_spirit(m)             (GET_MSTAT(m, spirit, spirit))
#define get_mana_regen(m)         (MSTAT(m, spirit, mana_regen))
#define get_health_regen(m)       (MSTAT(m, spirit, health_regen))
#define get_spirit_spell_power(m) (MSTAT(m, spirit, spell_power))
#define get_resist(m)             (MSTAT(m, spirit, resist))

#define get_stamina(m)      (GET_MSTAT(m, stamina, stamina))
#define get_health(m)       (MSTAT(m, stamina, health))
#define get_max_health(m)   (MSTAT(m, stamina, max_health))
#define get_block(m)        (MSTAT(m, stamina, block_chance))
#define get_crit_block(m)   (MSTAT(m, stamina, crit_block_chance))

#define get_attack_power(m) (get_strength_attack_power(m) + get_agility_attack_power(m))
#define get_spell_power(m)  (MSTAT(m, intelligence, spell_power) + MSTAT(m, spirit, spell_power))

/**
 * Strength increases attack power, armour pen, and parry chance.
 */
struct Strength
{
    int base_strength;
    int strength;
    int attack_power;
    int armour_pen;
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
bool strength_check(int roll, struct Mon*);
bool agility_check(int roll, struct Mon*);
bool intelligence_check(int roll, struct Mon*);
bool spirit_check(int roll, struct Mon*);
bool stamina_check(int roll, struct Mon*);

bool dodge_check(struct Mon* mon);
bool parry_check(struct Mon* mon);
bool block_check(struct Mon* mon);
bool crit_block_check(struct Mon* mon);

#endif
