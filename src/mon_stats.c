#include "mon_stats.h"

#include "log.h"
#include "monster.h"
#include "player.h"
#include "util.h"

#include <math.h>
#include <stdio.h>

const int   c_dodge_mod        = 4;
const int   c_parry_mod        = 8;
const float c_block_mod        = 1.1f;
const int   c_block_mod_2      = 3;
const int   c_crit_block_mod   = 3;
const float c_crit_mod         = 1.5f;
const float c_armpen_mod       = 200.0f;
const float c_spellpen_mod     = 400.0f;
const float c_health_regen_mod = 0.01f;
const float c_mana_regen_mod   = 0.02f;
const float c_resist_mod       = 0.2f;
const int   c_health_mod       = 3;
const int   c_mana_mod         = 4;

const int   c_strength_attack_power_mod    = 2;
const int   c_agility_attack_power_mod     = 1;
const int   c_intelligence_spell_power_mod = 2;
const int   c_spirit_spell_power_mod       = 1;

#define max(x, y) (((x) > (y) ? (x) : (y)))

static float        _get_mod(struct Mon* mon, short stat);
static inline int   _calc_strength_attack_power(struct Mon* mon);
static inline float _armpen_divisor(struct Mon* mon);
static inline int   _calc_armpen(struct Mon* mon);
static inline float _calc_parry(struct Mon* mon);
static void         _update_strength(struct Mon* mon);
static inline int   _calc_agility_attack_power(struct Mon* mon);
static inline float _calc_crit(struct Mon* mon);
static inline float _calc_dodge(struct Mon* mon);
static void         _update_agility(struct Mon* mon);
static inline int   _calc_max_mana(struct Mon* mon);
static inline int   _calc_intelligence_spell_power(struct Mon* mon);
static inline float _calc_spell_crit(struct Mon* mon);
static inline float _spellpen_divisor(struct Mon* mon);
static inline int   _calc_spell_pen(struct Mon* mon);
static void         _update_intelligence(struct Mon* mon);
static inline int   _calc_spirit_spell_power(struct Mon* mon);
static inline int   _calc_health_regen(struct Mon* mon);
static inline int   _calc_mana_regen(struct Mon* mon);
static inline int   _calc_resist(struct Mon* mon);
static void         _update_spirit(struct Mon* mon);
static inline int   _calc_max_health(struct Mon* mon);
static inline float _calc_block(struct Mon* mon);
static inline float _calc_crit_block(struct Mon* mon);
static void         _update_stamina(struct Mon* mon);

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

static inline int _calc_strength_attack_power(struct Mon* mon)
{
    return ((get_strength(mon) * _get_mod(mon, STAT_STRENGTH)) * c_strength_attack_power_mod);
}

static inline float _armpen_divisor(struct Mon* mon)
{
    return c_armpen_mod * (1.0f - ((float)get_strength(mon) / 999.0f));
}

static inline int _calc_armpen(struct Mon* mon)
{
    return (int) max(floor(((get_strength(mon) * get_strength(mon)) * _get_mod(mon, STAT_STRENGTH)) / _armpen_divisor(mon)) - 1.0f, 0.0f);
}

static inline float _calc_parry(struct Mon* mon)
{
    return atan(get_strength(mon) * _get_mod(mon, STAT_STRENGTH) * INV_STAT_MAX) / c_parry_mod;
}

/**
 * Increase the stats governed by strength
 */
static void _update_strength(struct Mon* mon)
{
    get_strength_attack_power(mon) = _calc_strength_attack_power(mon);
    get_parry(mon)                 = _calc_parry(mon);
    get_armour_pen(mon)            = _calc_armpen(mon);
}

static inline int _calc_agility_attack_power(struct Mon* mon)
{
    return (get_agility(mon) * _get_mod(mon, STAT_AGILITY) * c_agility_attack_power_mod);
}

static inline float _calc_crit(struct Mon* mon)
{
    return atan(get_agility(mon) * _get_mod(mon, STAT_AGILITY) * INV_STAT_MAX) / c_crit_mod;
}

static inline float _calc_dodge(struct Mon* mon)
{
    return atan(get_agility(mon) * _get_mod(mon, STAT_AGILITY) * INV_STAT_MAX) / c_dodge_mod;
}

/**
 * Increase the stats governed by agility
 */
static void _update_agility(struct Mon* mon)
{
    get_agility_attack_power(mon) = _calc_agility_attack_power(mon);
    get_crit(mon)                 = _calc_crit(mon);
    get_dodge(mon)                = _calc_dodge(mon);
}

static inline int _calc_max_mana(struct Mon* mon)
{
    return (get_intelligence(mon) * _get_mod(mon, STAT_INTELLIGENCE) * c_mana_mod);
}

static inline int _calc_intelligence_spell_power(struct Mon* mon)
{
    return (get_intelligence(mon) * _get_mod(mon, STAT_INTELLIGENCE) * c_intelligence_spell_power_mod);
}

static inline float _calc_spell_crit(struct Mon* mon)
{
    return atan(get_intelligence(mon) * _get_mod(mon, STAT_INTELLIGENCE) * INV_STAT_MAX) / c_crit_mod;
}

static inline float _spellpen_divisor(struct Mon* mon)
{
    return c_spellpen_mod * (1.0f - ((float)get_intelligence(mon) / 999.0f));
}

static inline int _calc_spell_pen(struct Mon* mon)
{
    return (int) max(floor(((get_intelligence(mon) * get_intelligence(mon)) * _get_mod(mon, STAT_INTELLIGENCE)) / _spellpen_divisor(mon)) - 1.0f, 0.0f);
}

/**
 * Increase the stats governed by intelligence
 */
static void _update_intelligence(struct Mon* mon)
{
    float mana_percent = get_max_mana(mon) > 0 ? (get_mana(mon) / get_max_mana(mon)) : 1.0f;

    get_max_mana(mon)                 = _calc_max_mana(mon);
    get_mana(mon)                     = get_max_mana(mon) * mana_percent;
    get_intelligence_spell_power(mon) = _calc_intelligence_spell_power(mon);
    get_spell_crit(mon)               = _calc_spell_crit(mon);
    get_spell_pen(mon)                = _calc_spell_pen(mon);
}

static inline int _calc_spirit_spell_power(struct Mon* mon)
{
    return (get_spirit(mon) * _get_mod(mon, STAT_SPIRIT) * c_spirit_spell_power_mod);
}

static inline int _calc_health_regen(struct Mon* mon)
{
    return (get_spirit(mon) * _get_mod(mon, STAT_SPIRIT) * c_health_regen_mod);
}

static inline int _calc_mana_regen(struct Mon* mon)
{
    return (get_spirit(mon) * _get_mod(mon, STAT_SPIRIT) * c_mana_regen_mod);
}

static inline int _calc_resist(struct Mon* mon)
{
    return (get_spirit(mon) * _get_mod(mon, STAT_SPIRIT) * c_resist_mod);
}

/**
 * Increase the stats governed by spirit
 */
static void _update_spirit(struct Mon* mon)
{
    get_spirit_spell_power(mon) = _calc_spirit_spell_power(mon);
    get_mana_regen(mon)         = _calc_mana_regen(mon);
    get_health_regen(mon)       = _calc_health_regen(mon);
    get_resist(mon)             = _calc_resist(mon);
}

static inline int _calc_max_health(struct Mon* mon)
{
    return (get_stamina(mon) * _get_mod(mon, STAT_STAMINA) * c_health_mod);
}

static inline float _calc_block(struct Mon* mon)
{
    return log_base((get_stamina(mon) / c_block_mod_2) + 1, STAT_MAX) / c_block_mod;
}

static inline float _calc_crit_block(struct Mon* mon)
{
    return _calc_block(mon) / c_crit_block_mod;
}

/**
 * Increase the stats governed by stamina
 */
static void _update_stamina(struct Mon* mon)
{
    float health_percent = get_max_health(mon) > 0 ? (get_health(mon) / get_max_health(mon)) : 1.0f;

    get_max_health(mon) = _calc_max_health(mon);
    get_health(mon)     = get_max_health(mon) * health_percent;
    get_block(mon)      = _calc_block(mon);
    get_crit_block(mon) = _calc_crit_block(mon);
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

inline bool strength_check(int roll, struct Mon* mon)
{
   return roll < get_strength(mon);
}

inline bool agility_check(int roll, struct Mon* mon)
{
    return roll < get_agility(mon);
}

inline bool intelligence_check(int roll, struct Mon* mon)
{
    return roll < get_intelligence(mon);
}

inline bool spirit_check(int roll, struct Mon* mon)
{
    return roll < get_spirit(mon);
}

inline bool stamina_check(int roll, struct Mon* mon)
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

bool crit_block_check(struct Mon* mon)
{
    float roll = roll_d100f();

    log_format_msg(DEBUG, "Critical block check. Rolled: %5.2f (needed: > %5.2f)", roll * 100.0f, (1.0f - get_crit_block(mon)) * 100.0f);

    return roll > (1.0f - get_crit_block(mon));
}
