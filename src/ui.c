#include "ui.h"

#include "class.h"
#include "colour.h"
#include "globals.h"
#include "inventory.h"
#include "log.h"
#include "map.h"
#include "mon.h"
#include "ncurses_ext.h"
#include "object.h"
#include "player.h"
#include "race.h"
#include "stats.h"

#include <ncurses.h>
#include <stdio.h>
#include <string.h>

#define STATUS_X 0
#define STATUS_Y 45
#define STATUS_W 80

void display_main_screen(void)
{
    display_map();
    display_char_status();
}

/*
 * Prints a simple view of the player's stats at the bottom of the game area.
 */
void display_char_status(void)
{
    char tmp[STATUS_W];
    sprintf(
        tmp, "%s\tHP: %d/%d  MP: %d/%d\tStr:%d Agi:%d Int:%d Spi:%d Sta:%d", you->name,
        HP(you->mon), MAXHP(you->mon), MANA(you->mon), MAXMANA(you->mon),
        PSTAT(strength, strength), PSTAT(agility, agility), PSTAT(intelligence, intelligence), PSTAT(spirit, spirit), PSTAT(stamina, stamina)
    );

    mvprintw(STATUS_Y, STATUS_X, tmp);
}

/*
 * Displays a detailed view of the player's stats.
 */
void display_char_info_screen(void)
{
    clear();

    mvprintw_xy(1, 0, "Name:  %s", you->name);
    mvprintw_xy(1, 1, "Class: %s", you->cls->name);
    mvprintw_xy(1, 2, "Race:  %s", you->race->noun);

    mvprintwa(1, 5, A_BOLD, "Strength       Agility        Intelligence   Spirit         Stamina");
    mvprintw_xy(1, 6, "AP:     %05d  AP:     %05d  Mana:   %05d  HRegen: %03d/t  HP:         %05d", PSTAT(strength, attack_power), PSTAT(agility, attack_power), PSTAT(intelligence, max_mana), PSTAT(spirit, health_regen), PSTAT(stamina, max_health));
    mvprintw_xy(1, 7, "ArmPen: %03d  Crit%:  %05.2f  SpePen: %05d  MRegen: %03d/t  Block%:     %05.2f", PSTAT(strength, armour_pen), PSTAT(agility, crit_chance) * 100.f, PSTAT(intelligence, spell_pen), PSTAT(spirit, mana_regen), PSTAT(stamina, block_chance) * 100.f);
    mvprintw_xy(1, 8, "Parry%: %05.2f  Dodge%: %05.2f  SP:     %05d  SP:     %05d  CritBlock%: %05.2f", PSTAT(strength, parry_chance) * 100.f, PSTAT(agility, dodge_chance) * 100.f, PSTAT(intelligence, spell_power), PSTAT(spirit, spell_power), PSTAT(stamina, crit_block_chance) * 100.f);
    mvprintw_xy(1, 9, "                              SCrit%: %05.2f  SRes%:  %05.2f", PSTAT(intelligence, spell_crit_chance) * 100.f, PSTAT(spirit, resist) * 100.f);

    mvprintw_xy(1, screen_rows-1, "q = close inventory");
}

/**
 * Displays items in the player's inventory
 */
void display_char_inventory(struct UIList* inv_list)
{
    int y;
    int displayable_rows = screen_rows - 4;

    clear();

    y = 0;
    mvprintwa(1, y, A_BOLD, "Inventory");
    y += 2;

    struct Object* obj = (struct Object*)inv_list->head;
    while(obj && y < displayable_rows)
    {
        if(obj == inv_list->current_selection)
        {
            mvprintwa(1, y++, COLOR_PAIR(30), "%s", obj->name);
        }
        else
        {
            mvprintw_xy(1, y++, "%s", obj->name);
        }

        obj = obj->next;
    }

    mvprintw_xy(1, screen_rows-1, "q: close inventory");
}
