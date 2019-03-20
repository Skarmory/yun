#include "stats.h"

#include "log.h"
#include "mon.h"
#include "player.h"
#include "util.h"

#include <math.h>
#include <stdio.h>

const int   c_dodge_mod      = 4;
const int   c_parry_mod      = 8;
const float c_block_mod      = 1.1f;
const int   c_block_mod_2    = 3;
const int   c_crit_block_mod = 3;
const float c_crit_mod       = 1.5f;
const float c_armpen_mod     = 200.0f;

#define max(x, y) (((x) > (y) ? (x) : (y)))

static inline float _get_mod(struct Mon* mon, short stat);
static inline float _armpen_divisor(struct Mon* mon);
static inline float _calc_dodge(struct Mon* mon);
static inline float _calc_parry(struct Mon* mon);
static inline float _calc_block(struct Mon* mon);
static inline float _calc_crit_block(struct Mon* mon);
static inline float _calc_crit(struct Mon* mon);
static inline int   _calc_armpen(struct Mon* mon);

static inline float _armpen_divisor(struct Mon* mon)
{
    return c_armpen_mod * (1.0f - ((float)get_strength(mon) / 999.0f));
}

static inline float _calc_dodge(struct Mon* mon)
{
    return atan(get_agility(mon) * _get_mod(mon, STAT_AGILITY) * INV_STAT_MAX) / c_dodge_mod;
}

static inline float _calc_parry(struct Mon* mon)
{
    return atan(get_strength(mon) * _get_mod(mon, STAT_STRENGTH) * INV_STAT_MAX) / c_parry_mod;
}

static inline float _calc_block(struct Mon* mon)
{
    return log_base((get_stamina(mon) / c_block_mod_2) + 1, STAT_MAX) / c_block_mod;
}

static inline float _calc_crit_block(struct Mon* mon)
{
    return _calc_block(mon) / c_crit_block_mod;
}

static inline float _calc_crit(struct Mon* mon)
{
    return atan(get_agility(mon) * _get_mod(mon, STAT_AGILITY) * INV_STAT_MAX) / c_crit_mod;
}

static inline int _calc_armpen(struct Mon* mon)
{
    return (int) max(floor(((get_strength(mon) * get_strength(mon)) * _get_mod(mon, STAT_STRENGTH)) / _armpen_divisor(mon)) - 1.0f, 0.0f);
}

/**
 * Get the stat scalar mod
 */
static float _get_mod(struct Mon* mon, short stat)
{
    switch(stat)
    {
        case STAT_STRENGTH:
            return mon->stats.strength.scale;
        case STAT_AGILITY:
            return mon->stats.agility.scale;
        case STAT_INTELLIGENCE:
            return mon->stats.intelligence.scale;
        case STAT_SPIRIT:
            return mon->stats.spirit.scale;
        case STAT_STAMINA:
            return mon->stats.stamina.scale;
    }

    return 0.0f;
}

/**
 * Increase the stats governed by strength
 */
static void _update_strength(struct Mon* mon)
{
    int str = MSTAT(mon, strength, strength);
    float invstr = str * INV_STAT_MAX;
    float mod = _get_mod(mon, STAT_STRENGTH);

    MSTAT(mon, strength, attack_power) = str * mod *  0.2f;
    get_parry(mon) = _calc_parry(mon);
    get_armour_pen(mon) = _calc_armpen(mon);
}

/**
 * Increase the stats governed by agility
 */
static void _update_agility(struct Mon* mon)
{
    int agi = MSTAT(mon, agility, agility);
    float invagi = agi * INV_STAT_MAX;
    float mod = _get_mod(mon, STAT_AGILITY);

    MSTAT(mon, agility, attack_power) = agi * mod *  0.2f;
    MSTAT(mon, agility, crit_chance) = invagi * mod * 0.75f;
    get_dodge(mon) = _calc_dodge(mon);
}

/**
 * Increase the stats governed by intelligence
 */
static void _update_intelligence(struct Mon* mon)
{
    int in = MSTAT(mon, intelligence, intelligence);
    float invin = in * INV_STAT_MAX;
    float mod = _get_mod(mon, STAT_INTELLIGENCE);

    float mana_percent;
    if(MSTAT(mon,intelligence,max_mana) == 0)
        mana_percent = 1.0f;
    else
        mana_percent = MSTAT(mon, intelligence, mana) / MSTAT(mon, intelligence, max_mana);

    MSTAT(mon, intelligence, max_mana) = in * mod;
    MSTAT(mon, intelligence, mana) = MSTAT(mon, intelligence, max_mana) * mana_percent;
    MSTAT(mon, intelligence, spell_power) = in * mod * 0.2f;
    MSTAT(mon, intelligence, spell_crit_chance) = invin * mod * 0.75f;
    MSTAT(mon, intelligence, spell_pen) = invin * mod * 0.75f;
}

/**
 * Increase the stats governed by spirit
 */
static void _update_spirit(struct Mon* mon)
{
    int spi = MSTAT(mon, spirit, spirit);
    float invspi = spi * INV_STAT_MAX;
    float mod = _get_mod(mon, STAT_SPIRIT);

    MSTAT(mon, spirit, spell_power) = spi * mod * 0.2f;
    MSTAT(mon, spirit, mana_regen) = spi * mod * 0.1f;
    MSTAT(mon, spirit, health_regen) = spi * mod * 0.1f;
    MSTAT(mon, spirit, resist) = invspi * mod * 0.75f;
}

/**
 * Increase the stats governed by stamina
 */
static void _update_stamina(struct Mon* mon)
{
    int sta = MSTAT(mon, stamina, stamina);
    float invsta = sta * INV_STAT_MAX;
    float mod = _get_mod(mon, STAT_STAMINA);

    float health_percent;
    if(MSTAT(mon,stamina,max_health) == 0)
        health_percent = 1.0f;
    else
        health_percent = MSTAT(mon, stamina, health) / MSTAT(mon, stamina, max_health);

    MSTAT(mon, stamina, max_health) = sta * mod;
    MSTAT(mon, stamina, health) = MSTAT(mon, stamina, max_health) * health_percent;
    MSTAT(mon, stamina, block_chance) = invsta * mod * 0.75f;
    MSTAT(mon, stamina, crit_block_chance) = invsta * mod * 0.5f;
    get_block(mon) = _calc_block(mon);
}

/**
 * Sets given stat to given value for given mon
 *
 * Updates the governed stats of given stat of given mon
 */
void set_stat(struct Mon* mon, int stat, int amount)
{
    switch(stat)
    {
        case STAT_STRENGTH:
            MSTAT(mon, strength, strength) = amount;
            MSTAT(mon, strength, base_strength) = amount;
            _update_strength(mon);
            break;
        case STAT_AGILITY:
            MSTAT(mon, agility, agility) = amount;
            MSTAT(mon, agility, base_agility) = amount;
            _update_agility(mon);
            break;
        case STAT_INTELLIGENCE:
            MSTAT(mon, intelligence, intelligence) = amount;
            MSTAT(mon, intelligence, base_intelligence) = amount;
            _update_intelligence(mon);
            break;
        case STAT_SPIRIT:
            MSTAT(mon, spirit, spirit) = amount;
            MSTAT(mon, spirit, base_spirit) = amount;
            _update_spirit(mon);
            break;
        case STAT_STAMINA:
            MSTAT(mon, stamina, stamina) = amount;
            MSTAT(mon, stamina, base_stamina) = amount;
            _update_stamina(mon);
            break;
    }
}

/**
 * Modifies given stat by given amount for given mon.
 *
 * If base, then update the base stat by this amount
 *
 * Updates governed stats
 */
void add_stat(struct Mon* mon, int stat, int amount, bool base)
{
    switch(stat)
    {
        case STAT_STRENGTH:
            if(base) MSTAT(mon, strength, base_strength) += amount;
            MSTAT(mon, strength, strength) += amount;
            _update_strength(mon);
            break;
        case STAT_AGILITY:
            if(base) MSTAT(mon, agility, base_agility) += amount;
            MSTAT(mon, agility, agility) += amount;
            _update_agility(mon);
            break;
        case STAT_INTELLIGENCE:
            if(base) MSTAT(mon, intelligence, base_intelligence) += amount;
            MSTAT(mon, intelligence, intelligence) += amount;
            _update_intelligence(mon);
            break;
        case STAT_SPIRIT:
            if(base) MSTAT(mon, spirit, base_spirit) += amount;
            MSTAT(mon, spirit, spirit) += amount;
            _update_spirit(mon);
            break;
        case STAT_STAMINA:
            if(base) MSTAT(mon, stamina, base_stamina) += amount;
            MSTAT(mon, stamina, stamina) += amount;
            _update_stamina(mon);
            break;
    }
}

bool inline strength_check(int roll, struct Mon* mon)
{
   return roll < get_strength(mon);
}

bool inline agility_check(int roll, struct Mon* mon)
{
    return roll < get_agility(mon);
}

bool inline intelligence_check(int roll, struct Mon* mon)
{
    return roll < get_intelligence(mon);
}

bool inline spirit_check(int roll, struct Mon* mon)
{
    return roll < get_spirit(mon);
}

bool inline stamina_check(int roll, struct Mon* mon)
{
    return roll < get_stamina(mon);
}

/**
 * Perform a stat check for given stat and return whether succeeded or failed
 *
 * Stat checks are d100-based (% based).
 *
 * Rolling a 1 is an automatic fail, and rolling 100 is an automatic success
 */
bool stat_check(struct Mon* mon, int stat)
{
    int roll = roll_d100();

    if(roll == 1)
        return false;

    if(roll == 100)
        return true;

   switch(stat)
   {
       case STAT_STRENGTH:
           return strength_check(roll, mon);
       case STAT_AGILITY:
           return agility_check(roll, mon);
       case STAT_INTELLIGENCE:
           return intelligence_check(roll, mon);
       case STAT_SPIRIT:
           return spirit_check(roll, mon);
       case STAT_STAMINA:
           return stamina_check(roll, mon);
   }

   return false;
}

bool dodge_check(struct Mon* mon)
{
    float roll = roll_d100f();

    log_format_msg(DEBUG, "Dodge check. Rolled: %5.2f (needed: > %5.2f)", roll * 100.0f, (1.0f - get_dodge(mon)) * 100.0f);

    return roll > (1.0f - get_dodge(mon));
}

bool parry_check(struct Mon* mon)
{
    float roll = roll_d100f();

    log_format_msg(DEBUG, "Parry check. Rolled: %5.2f (needed: > %5.2f)", roll * 100.0f, (1.0f - get_parry(mon)) * 100.0f);

    return roll > (1.0f - get_parry(mon));
}

bool block_check(struct Mon* mon)
{
    float roll = roll_d100f();

    log_format_msg(DEBUG, "Block check. Rolled: %5.2f (needed: > %5.2f)", roll * 100.0f, (1.0f - get_block(mon)) * 100.0f);

    return roll > (1.0f - get_block(mon));
}
