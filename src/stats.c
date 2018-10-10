#include "stats.h"

#include "log.h"
#include "mon.h"
#include "player.h"
#include "util.h"

#include <math.h>
#include <stdio.h>

#define DODGE_MOD 4
#define PARRY_MOD 3
#define BLOCK_MOD 1.1f
#define BLOCK_MOD2 3

#define calc_dodge(mon)      (atan(get_agility(mon) * _get_mod(mon, AGILITY) * INV_STAT_MAX) / DODGE_MOD)
#define calc_parry(mon)      (atan(get_strength(mon) * _get_mod(mon, STRENGTH) * INV_STAT_MAX) / PARRY_MOD)
#define calc_block(mon)      (log_base((get_stamina(mon) / BLOCK_MOD2) + 1, STAT_MAX) / BLOCK_MOD)
#define calc_crit_block(mon) (calc_block(mon))

/**
 * Get the stat scalar mod
 */
float _get_mod(struct Mon* mon, short stat)
{
    switch(stat)
    {
        case STRENGTH:
            return mon->stats.strength.scale;
        case AGILITY:
            return mon->stats.agility.scale;
        case INTELLIGENCE:
            return mon->stats.intelligence.scale;
        case SPIRIT:
            return mon->stats.spirit.scale;
        case STAMINA:
            return mon->stats.stamina.scale;
        default:
            return 0.0f; // How did you even get here?
    }
}

/**
 * Increase the stats governed by strength
 */
void _update_strength(struct Mon* mon)
{
    int str = MSTAT(mon, strength, strength);
    float invstr = str * INV_STAT_MAX;
    float mod = _get_mod(mon, STRENGTH);

    MSTAT(mon, strength, attack_power) = str * mod *  0.2f;
    MSTAT(mon, strength, armour_pen) = invstr * mod * 0.75f;
    get_parry(mon) = calc_parry(mon);
}

/**
 * Increase the stats governed by agility
 */
void _update_agility(struct Mon* mon)
{
    int agi = MSTAT(mon, agility, agility);
    float invagi = agi * INV_STAT_MAX;
    float mod = _get_mod(mon, AGILITY);

    MSTAT(mon, agility, attack_power) = agi * mod *  0.2f;
    MSTAT(mon, agility, crit_chance) = invagi * mod * 0.75f;
    get_dodge(mon) = calc_dodge(mon);
}

/**
 * Increase the stats governed by intelligence
 */
void _update_intelligence(struct Mon* mon)
{
    int in = MSTAT(mon, intelligence, intelligence);
    float invin = in * INV_STAT_MAX;
    float mod = _get_mod(mon, INTELLIGENCE);

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
void _update_spirit(struct Mon* mon)
{
    int spi = MSTAT(mon, spirit, spirit);
    float invspi = spi * INV_STAT_MAX;
    float mod = _get_mod(mon, SPIRIT);

    MSTAT(mon, spirit, spell_power) = spi * mod * 0.2f;
    MSTAT(mon, spirit, mana_regen) = spi * mod * 0.1f;
    MSTAT(mon, spirit, health_regen) = spi * mod * 0.1f;
    MSTAT(mon, spirit, resist) = invspi * mod * 0.75f;
}

/**
 * Increase the stats governed by stamina
 */
void _update_stamina(struct Mon* mon)
{
    int sta = MSTAT(mon, stamina, stamina);
    float invsta = sta * INV_STAT_MAX;
    float mod = _get_mod(mon, STAMINA);

    float health_percent;
    if(MSTAT(mon,stamina,max_health) == 0)
        health_percent = 1.0f;
    else
        health_percent = MSTAT(mon, stamina, health) / MSTAT(mon, stamina, max_health);

    MSTAT(mon, stamina, max_health) = sta * mod;
    MSTAT(mon, stamina, health) = MSTAT(mon, stamina, max_health) * health_percent;
    MSTAT(mon, stamina, block_chance) = invsta * mod * 0.75f;
    MSTAT(mon, stamina, crit_block_chance) = invsta * mod * 0.5f;
    get_block(mon) = calc_block(mon);
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
        case STRENGTH:
            MSTAT(mon, strength, strength) = amount;
            MSTAT(mon, strength, base_strength) = amount;
            _update_strength(mon);
            break;
        case AGILITY:
            MSTAT(mon, agility, agility) = amount;
            MSTAT(mon, agility, base_agility) = amount;
            _update_agility(mon);
            break;
        case INTELLIGENCE:
            MSTAT(mon, intelligence, intelligence) = amount;
            MSTAT(mon, intelligence, base_intelligence) = amount;
            _update_intelligence(mon);
            break;
        case SPIRIT:
            MSTAT(mon, spirit, spirit) = amount;
            MSTAT(mon, spirit, base_spirit) = amount;
            _update_spirit(mon);
            break;
        case STAMINA:
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
        case STRENGTH:
            if(base) MSTAT(mon, strength, base_strength) += amount;
            MSTAT(mon, strength, strength) += amount;
            _update_strength(mon);
            break;
        case AGILITY:
            if(base) MSTAT(mon, agility, base_agility) += amount;
            MSTAT(mon, agility, agility) += amount;
            _update_agility(mon);
            break;
        case INTELLIGENCE:
            if(base) MSTAT(mon, intelligence, base_intelligence) += amount;
            MSTAT(mon, intelligence, intelligence) += amount;
            _update_intelligence(mon);
            break;
        case SPIRIT:
            if(base) MSTAT(mon, spirit, base_spirit) += amount;
            MSTAT(mon, spirit, spirit) += amount;
            _update_spirit(mon);
            break;
        case STAMINA:
            if(base) MSTAT(mon, stamina, base_stamina) += amount;
            MSTAT(mon, stamina, stamina) += amount;
            _update_stamina(mon);
            break;
    }
}

#define STRENGTH_CHECK(x) ( x < GET_MSTAT(mon, strength, strength)  )
#define AGILITY_CHECK(x) ( x < GET_MSTAT(mon, agility, agility)  )
#define INTELLIGENCE_CHECK(x) ( x < GET_MSTAT(mon, intelligence, intelligence)  )
#define SPIRIT_CHECK(x) ( x < GET_MSTAT(mon, spirit, spirit)  )
#define STAMINA_CHECK(x) ( x < GET_MSTAT(mon, stamina, stamina)  )

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
       case STRENGTH:
           return STRENGTH_CHECK(roll);
       case AGILITY:
           return AGILITY_CHECK(roll);
       case INTELLIGENCE:
           return INTELLIGENCE_CHECK(roll);
       case SPIRIT:
           return SPIRIT_CHECK(roll);
       case STAMINA:
           return STAMINA_CHECK(roll);
        default:
           return false;
   }
}

bool dodge_check(struct Mon* mon)
{
    float roll = roll_d100f();

    log_format_msg("Dodge check. Rolled: %5.2f (needed: > %5.2f)", DEBUG, roll * 100.0f, (1.0f - get_dodge(mon)) * 100.0f);

    return roll > (1.0f - get_dodge(mon));
}

bool parry_check(struct Mon* mon)
{
    float roll = roll_d100f();

    log_format_msg("Parry check. Rolled: %5.2f (needed: > %5.2f)", DEBUG, roll * 100.0f, (1.0f - get_parry(mon)) * 100.0f);

    return roll > (1.0f - get_parry(mon));
}

bool block_check(struct Mon* mon)
{
    float roll = roll_d100f();

    log_format_msg("Block check. Rolled: %5.2f (needed: > %5.2f)", DEBUG, roll * 100.0f, (1.0f - get_block(mon)) * 100.0f);

    return roll > (1.0f - get_block(mon));
}
