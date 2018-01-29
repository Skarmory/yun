#include "ui.h"
#include "player.h"
#include "map.h"
#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include "log.h"

#define STATUS_X 0
#define STATUS_Y 45
#define STATUS_W 80

void display_char_status(void)
{
    char tmp[STATUS_W];
    sprintf(tmp, "%s\t", you->name);
    sprintf(tmp, "%sHP: %d/%d  MP: %d/%d\t", tmp, HP(you->mon), MAXHP(you->mon), MANA(you->mon), MAXMANA(you->mon));
    sprintf(tmp, "%sStr:%d Agi:%d Int:%d Spi:%d Sta:%d", tmp, PSTAT(strength, strength), PSTAT(agility, agility), PSTAT(intelligence, intelligence), PSTAT(spirit, spirit), PSTAT(stamina, stamina));

    mvprintw(STATUS_Y, STATUS_X, tmp);
}

void display_char_info_screen(void)
{
    clear();

    attron(A_BOLD);
    mvprintw(0, 0, "Strength              Agility               Intelligence         Spirit                  Stamina");
    attroff(A_BOLD);
    mvprintw(1, 0, "Attack Power: %d       Attack Power: %d       Mana:        %d       HP Regen:     %d/turn    Hit Points:        %d", PSTAT(strength, attack_power), PSTAT(agility, attack_power), PSTAT(intelligence, max_mana), PSTAT(spirit, health_regen), PSTAT(stamina, max_health));
    mvprintw(2, 0, "Armour Pen:   %2.2f%%   Crit Chance:  %2.2f%%   Spell Pen:   %d       Mana Regen    %d/turn    Block Chance:      %2.2f%%", PSTAT(strength, armour_pen) * 100.f, PSTAT(agility, crit_chance) * 100.f, PSTAT(intelligence, spell_pen), PSTAT(spirit, mana_regen), PSTAT(stamina, block_chance) * 100.f);
    mvprintw(3, 0, "Parry Chance: %2.2f%%   Dodge Chance: %2.2f%%   Spell Power: %d       Spell Power:  %d         Crit Block Chance: %2.2f%%", PSTAT(strength, parry_chance) * 100.f, PSTAT(agility, dodge_chance) * 100.f, PSTAT(intelligence, spell_power), PSTAT(spirit, spell_power), PSTAT(stamina, crit_block_chance) * 100.f);
    mvprintw(4, 0, "                                            Spell Crit:  %2.2f%%   Spell Resist: %2.2f%%     Block Amount:      %d", PSTAT(intelligence, spell_crit_chance) * 100.f, PSTAT(spirit, resist) * 100.f, PSTAT(stamina, block_amount) );

    getch();

    clear();

    display_map();
    display_char_status();
}
