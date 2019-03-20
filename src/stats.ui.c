#include "stats.ui.h"

#include <ncurses.h>

#include "class.h"
#include "globals.h"
#include "mon.h"
#include "ncurses_ext.h"
#include "player.h"
#include "race.h"
#include "stats.h"

void _display_stats(struct Mon* mon)
{
    clear();

    mvprintw_xy(1, 0, "Name:  %s", you->name);
    mvprintw_xy(1, 1, "Class: %s", you->cls->name);
    mvprintw_xy(1, 2, "Race:  %s", you->race->noun);

    mvprintwa_xy(1, 5, A_BOLD, "Strength       Agility        Intelligence   Spirit         Stamina");
    mvprintw_xy(1,  6, "AP:     %05d  AP:     %05d  Mana:   %05d  HRegen: %03d/t  HP:         %05d", PSTAT(strength, attack_power), PSTAT(agility, attack_power), PSTAT(intelligence, max_mana), PSTAT(spirit, health_regen), PSTAT(stamina, max_health));
    mvprintw_xy(1,  7, "ArmPen: %03d  Crit%:  %05.2f  SpePen: %05d  MRegen: %03d/t  Block%:     %05.2f", PSTAT(strength, armour_pen), PSTAT(agility, crit_chance) * 100.f, PSTAT(intelligence, spell_pen), PSTAT(spirit, mana_regen), PSTAT(stamina, block_chance) * 100.f);
    mvprintw_xy(1,  8, "Parry%: %05.2f  Dodge%: %05.2f  SP:     %05d  SP:     %05d  CritBlock%: %05.2f", PSTAT(strength, parry_chance) * 100.f, PSTAT(agility, dodge_chance) * 100.f, PSTAT(intelligence, spell_power), PSTAT(spirit, spell_power), PSTAT(stamina, crit_block_chance) * 100.f);
    mvprintw_xy(1,  9, "                              SCrit%: %05.2f  SRes%:  %05.2f", PSTAT(intelligence, spell_crit_chance) * 100.f, PSTAT(spirit, resist) * 100.f);

    mvprintw_xy(1, screen_rows-1, "q = close inventory");
}

bool character_screen_handler(void)
{
    char in;
    do
    {
        _display_stats(you->mon);
        in = getch();
    }
    while(in != 'q');

    return false;
}
