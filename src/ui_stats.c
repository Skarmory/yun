#include "ui_stats.h"

#include "globals.h"
#include "monster.h"
#include "mon_stats.h"
#include "player.h"
#include "player_class.h"
#include "player_race.h"
#include "term.h"

#include <stddef.h>

const int x_off_strength     = 1;
const int x_off_agility      = 17;
const int x_off_intelligence = 33;
const int x_off_spirit       = 50;
const int x_off_stamina      = 66;

void _display_stats(struct Mon* mon)
{
    term_draw_ftext(2, 0, NULL, NULL, 0, "Name: %s", you->name);
    term_draw_ftext(1, 1, NULL, NULL, 0, "Class: %s", you->cls->name);
    term_draw_ftext(2, 2, NULL, NULL, 0, "Race: %s", you->race->noun);

    int y;

    // Strength
    y = 5;
    term_draw_ftext(x_off_strength, y++, NULL, NULL, A_BOLD, "Strength");
    term_draw_ftext(x_off_strength, y++, NULL, NULL, 0, "AP:       %3d", PSTAT(strength, attack_power));
    term_draw_ftext(x_off_strength, y++, NULL, NULL, 0, "ArmPen:   %3d", PSTAT(strength, armour_pen));
    term_draw_ftext(x_off_strength, y++, NULL, NULL, 0, "Parry%: %5.2f", PSTAT(strength, parry_chance));

    // Agility
    y = 5;
    term_draw_ftext(x_off_agility, y++, NULL, NULL, A_BOLD, "Agility");
    term_draw_ftext(x_off_agility, y++, NULL, NULL, 0, "AP:       %3d", PSTAT(agility, attack_power));
    term_draw_ftext(x_off_agility, y++, NULL, NULL, 0, "Crit%:  %5.2f", PSTAT(agility, crit_chance));
    term_draw_ftext(x_off_agility, y++, NULL, NULL, 0, "Dodge%: %5.2f", PSTAT(agility, dodge_chance));

    // Intelligence
    y = 5;
    term_draw_ftext(x_off_intelligence, y++, NULL, NULL, A_BOLD, "Intelligence");
    term_draw_ftext(x_off_intelligence, y++, NULL, NULL, 0, "SP:        %3d", PSTAT(intelligence, spell_power));
    term_draw_ftext(x_off_intelligence, y++, NULL, NULL, 0, "SpCrit%: %5.2f", PSTAT(intelligence, spell_crit_chance));
    term_draw_ftext(x_off_intelligence, y++, NULL, NULL, 0, "Mana:      %3d", PSTAT(intelligence, max_mana));
    term_draw_ftext(x_off_intelligence, y++, NULL, NULL, 0, "SpPen:     %3d", PSTAT(intelligence, spell_pen));

    // Spirit
    y = 5;
    term_draw_ftext(x_off_spirit, y++, NULL, NULL, A_BOLD, "Spirit");
    term_draw_ftext(x_off_spirit, y++, NULL, NULL, 0, "SP:       %3d", PSTAT(spirit, spell_power));
    term_draw_ftext(x_off_spirit, y++, NULL, NULL, 0, "SpRes%: %5.2f", PSTAT(spirit, resist));
    term_draw_ftext(x_off_spirit, y++, NULL, NULL, 0, "HRegen:   %3d", PSTAT(spirit, health_regen));
    term_draw_ftext(x_off_spirit, y++, NULL, NULL, 0, "MRegen:   %3d", PSTAT(spirit, mana_regen));

    // Stamina
    y = 5;
    term_draw_ftext(x_off_stamina, y++, NULL, NULL, A_BOLD, "Stamina");
    term_draw_ftext(x_off_stamina, y++, NULL, NULL, 0, "HP:           %3d", PSTAT(stamina, max_health));
    term_draw_ftext(x_off_stamina, y++, NULL, NULL, 0, "Block%:     %5.2f", PSTAT(stamina, block_chance));
    term_draw_ftext(x_off_stamina, y++, NULL, NULL, 0, "CritBlock%: %5.2f", PSTAT(stamina, crit_block_chance));

    term_draw_text(1, screen_rows-1, NULL, NULL, 0, "q = close character screen");
}

bool character_screen_handler(void)
{
    char in;
    do
    {
        term_clear();
        _display_stats(you->mon);
        term_refresh();
        in = term_getch();
    }
    while(in != 'q');

    term_clear();

    return false;
}
