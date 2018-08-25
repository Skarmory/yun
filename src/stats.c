#include "stats.h"
#include "mon.h"
#include "log.h"
#include "player.h"
#include <stdio.h>

float _get_mod(struct Mon* mon, short stat)
{
    if(mon->stats.primary == stat)
        return PRIMARY_MOD;
    else if(mon->stats.secondary == stat)
        return SECONDARY_MOD;
    else if(mon->stats.tertiary == stat)
        return TERTIARY_MOD;
    else
        return OTHER_MOD;
}

void _update_strength(struct Mon* mon)
{
    int str = MSTAT(mon, strength, strength);
    float invstr = str * INV_STAT_MAX;
    float mod = _get_mod(mon, STRENGTH);

    MSTAT(mon, strength, attack_power) = str * mod *  0.2f;
    MSTAT(mon, strength, armour_pen) = invstr * mod * 0.75f;
    MSTAT(mon, strength, parry_chance) = invstr * mod * 0.5f;
}

void _update_agility(struct Mon* mon)
{
    int agi = MSTAT(mon, agility, agility);
    float invagi = agi * INV_STAT_MAX;
    float mod = _get_mod(mon, AGILITY);

    MSTAT(mon, agility, attack_power) = agi * mod *  0.2f;
    MSTAT(mon, agility, crit_chance) = invagi * mod * 0.75f;
    MSTAT(mon, agility, dodge_chance) = invagi * mod * 0.5f;
}

void _update_intelligence(struct Mon* mon)
{
    int in = MSTAT(mon, intelligence, intelligence);
    float invin = in * INV_STAT_MAX;
    float mod = _get_mod(mon, INTELLIGENCE);

    float mana_percent = MSTAT(mon, stamina, health) / MSTAT(mon, stamina, max_health);

    MSTAT(mon, intelligence, max_mana) = in * mod;
    MSTAT(mon, intelligence, mana) = MSTAT(mon, intelligence, max_mana) * mana_percent;
    MSTAT(mon, intelligence, spell_power) = in * mod * 0.2f;
    MSTAT(mon, intelligence, spell_crit_chance) = invin * mod * 0.75f;
    MSTAT(mon, intelligence, spell_pen) = invin * mod * 0.75f;
}

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

void _update_stamina(struct Mon* mon)
{
    int sta = MSTAT(mon, stamina, stamina);
    float invsta = sta * INV_STAT_MAX;
    float mod = _get_mod(mon, STAMINA);

    float health_percent = MSTAT(mon, stamina, health) / MSTAT(mon, stamina, max_health);

    MSTAT(mon, stamina, max_health) = sta * mod;
    MSTAT(mon, stamina, health) = MSTAT(mon, stamina, max_health) * health_percent;
    MSTAT(mon, stamina, block_chance) = invsta * mod * 0.75f;
    MSTAT(mon, stamina, crit_block_chance) = invsta * mod * 0.5f;
    MSTAT(mon, stamina, block_amount) = sta * mod * 0.2f;
}

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
