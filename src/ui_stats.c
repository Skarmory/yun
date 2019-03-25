#include "ui_stats.h"

#include <ncurses.h>

#include "globals.h"
#include "monster.h"
#include "mon_stats.h"
#include "ncurses_ext.h"
#include "player.h"
#include "player_class.h"
#include "player_race.h"

const int x_off_strength     = 1;
const int x_off_agility      = 17;
const int x_off_intelligence = 33;
const int x_off_spirit       = 50;
const int x_off_stamina      = 66;

void _display_stats(struct Mon* mon)
{
    clear();

    mvprintw_xy(1, 0, "Name:  %s", you->name);
    mvprintw_xy(1, 1, "Class: %s", you->cls->name);
    mvprintw_xy(1, 2, "Race:  %s", you->race->noun);

    int y;

    // Strength
    y = 5;
    mvprintwa_xy(x_off_strength, y++, A_BOLD, "Strength");
    mvprintw_xy(x_off_strength, y++, "AP:       %3d", PSTAT(strength, attack_power));
    mvprintw_xy(x_off_strength, y++, "ArmPen:   %3d", PSTAT(strength, armour_pen));
    mvprintw_xy(x_off_strength, y++, "Parry%: %5.2f", PSTAT(strength, parry_chance));

    // Agility
    y = 5;
    mvprintwa_xy(x_off_agility, y++, A_BOLD, "Agility");
    mvprintw_xy(x_off_agility, y++, "AP:       %3d", PSTAT(agility, attack_power));
    mvprintw_xy(x_off_agility, y++, "Crit%:  %5.2f", PSTAT(agility, crit_chance));
    mvprintw_xy(x_off_agility, y++, "Dodge%: %5.2f", PSTAT(agility, dodge_chance));

    // Intelligence
    y = 5;
    mvprintwa_xy(x_off_intelligence, y++, A_BOLD, "Intelligence");
    mvprintw_xy(x_off_intelligence, y++, "SP:        %3d", PSTAT(intelligence, spell_power));
    mvprintw_xy(x_off_intelligence, y++, "SpCrit%: %5.2f", PSTAT(intelligence, spell_crit_chance));
    mvprintw_xy(x_off_intelligence, y++, "Mana:      %3d", PSTAT(intelligence, max_mana));
    mvprintw_xy(x_off_intelligence, y++, "SpPen:     %3d", PSTAT(intelligence, spell_pen));

    // Spirit
    y = 5;
    mvprintwa_xy(x_off_spirit, y++, A_BOLD, "Spirit");
    mvprintw_xy(x_off_spirit, y++, "SP:       %3d", PSTAT(spirit, spell_power));
    mvprintw_xy(x_off_spirit, y++, "SpRes%: %5.2f", PSTAT(spirit, resist));
    mvprintw_xy(x_off_spirit, y++, "HRegen:   %3d", PSTAT(spirit, health_regen));
    mvprintw_xy(x_off_spirit, y++, "MRegen:   %3d", PSTAT(spirit, mana_regen));

    // Stamina
    y = 5;
    mvprintwa_xy(x_off_stamina, y++, A_BOLD, "Stamina");
    mvprintw_xy(x_off_stamina, y++, "HP:           %3d", PSTAT(stamina, max_health));
    mvprintw_xy(x_off_stamina, y++, "Block%:     %5.2f", PSTAT(stamina, block_chance));
    mvprintw_xy(x_off_stamina, y++, "CritBlock%: %5.2f", PSTAT(stamina, crit_block_chance));

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
