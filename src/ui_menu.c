#include "ui_menu.h"

#include "colour.h"
#include "input_keycodes.h"
#include "log.h"
#include "monster.h"
#include "mon_stats.h"
#include "mon_type.h"
#include "player.h"
#include "player_class.h"
#include "player_faction.h"
#include "player_race.h"
#include "util.h"

#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * Print out the character creation choices made so far
 */
void print_picked(void)
{
    int col;
    term_draw_ftext(1, 2, NULL, NULL, 0, "   name: %s", !you->name ? "unknown" : you->name);

    col = get_class_colour(you->cls);
    term_draw_text(2, 2, NULL, NULL, 0, "  class: ");
    term_draw_ftext(11, 2, COL(col), NULL, 0, "%s", !you->cls ? "not chosen" : you->cls->name);

    col = you->race ? you->race->faction == FA_ALLIANCE ? CLR_ALLIANCE : CLR_HORDE : CLR_DEFAULT;

    term_draw_text(2, 3, NULL, NULL, 0, "   race: ");
    term_draw_ftext(11, 3, COL(col), NULL, 0, "%s", !you->race ? "not chosen" : you->race->noun);

    term_draw_text(2, 4, NULL, NULL, 0, "faction: ");
    term_draw_ftext(11, 4, COL(col), NULL, 0, "%s", !you->faction ? "not chosen" : you->faction);
}

/**
 * Print character creation current choice to make
 */
void print_options(int what, short mask)
{
    int menu_col = 60, menu_row = 1;

    switch(what)
    {
        case PICK_CLASS:
            term_draw_text(menu_col, menu_row, NULL, NULL, 0, "Choose your class");
            menu_row += 2;
            for(int i = 0; i < 9; i++, menu_row++)
            {
                int col = get_class_colour(&classes[i]);
                term_draw_ftext(menu_col, menu_row, col != CLR_DEFAULT ? COL(col) : NULL, NULL, 0, "%c - %s", classes[i].hotkey, classes[i].name);
            }

            break;

        case PICK_RACE:
            term_draw_text(menu_col, menu_row, NULL, NULL, 0, "Choose your race");
            menu_row += 2;
            for(int i = 0; i < 8; i++)
            {
                if(races[i].selfmask & mask)
                {
                    int col = races[i].faction == FA_ALLIANCE ? CLR_ALLIANCE : CLR_HORDE;
                    term_draw_ftext(menu_col, menu_row, col != CLR_DEFAULT ? COL(col) : NULL, NULL, 0, "%c - %s", races[i].hotkey, races[i].noun);
                    you->faction = factions[races[i].faction];
                    menu_row++;
                }
            }

            break;

        case CONFIRM:
            term_draw_text(menu_col, menu_row, NULL, NULL, 0, "Confirm this character and start a new game?");
            menu_row += 2;
            term_draw_text(menu_col, menu_row++, NULL, NULL, 0, "y - Start game");
            term_draw_text(menu_col, menu_row++, NULL, NULL, 0, "n - Choose again");
            break;
    }

    menu_row++;
    term_draw_text(menu_col, menu_row, NULL, NULL, 0, "q - quit");

    term_refresh();
}

/**
 * Logic for picking a class
 *
 * Loops on blocking input until player makes a valid choice
 *
 * Set stats based on choice
 */
void pick_class(void)
{
    term_clear();

    print_picked();

    print_options(PICK_CLASS, 0);

    you->cls = (struct Class*)malloc(sizeof(struct Class));

    bool picked = false;

    do
    {
        enum KeyCode choice = get_key();
        if(choice == KEYCODE_y)
        {
            do_quit();
            return;
        }

        int cls_idx = get_class_idx(choice);
        if(cls_idx == -1)
            continue;

        *(you->cls) = classes[cls_idx];
        picked = true;
    }
    while(!picked);

}

/**
 * Logic for picking a race
 *
 * Loops on blocking input until player makes a valid choice
 *
 * Sets stats based on selection
 */
void pick_race(void)
{
    term_clear();

    print_picked();

    short allowed = you->cls->allow_races & 0xffff;

    print_options(PICK_RACE, allowed);

    you->race = (struct Race*)malloc(sizeof(struct Race));

    bool picked = false;

    do
    {
        enum KeyCode choice = get_key();
        if(choice == KEYCODE_q)
        {
            do_quit();
            return;
        }

        int mask = get_race_mask(choice);
        if(you->cls->allow_races & mask)
        {
            int race_idx = get_race_idx(choice);
            if(race_idx == -1)
                continue;

            char* faction_name = get_faction_name(get_faction_by_race(choice));

            *(you->race) = races[race_idx];
            you->faction = faction_name;
            picked = true;
        }
    }
    while(!picked);

    you->mon->type = get_race_montype(you->race->hotkey);
}

void _apply_stats(void)
{
    set_stat(you->mon, STAT_STRENGTH, you->mon->type->strength);
    set_stat(you->mon, STAT_AGILITY, you->mon->type->agility);
    set_stat(you->mon, STAT_INTELLIGENCE, you->mon->type->intelligence);
    set_stat(you->mon, STAT_SPIRIT, you->mon->type->spirit);
    set_stat(you->mon, STAT_STAMINA, you->mon->type->stamina);

    you->mon->stats.strength.scale = you->cls->strength_scale;
    you->mon->stats.agility.scale = you->cls->agility_scale;
    you->mon->stats.intelligence.scale = you->cls->intelligence_scale;
    you->mon->stats.spirit.scale = you->cls->spirit_scale;
    you->mon->stats.stamina.scale = you->cls->stamina_scale;

    add_stat(you->mon, STAT_STRENGTH, you->cls->strength_up, true);
    add_stat(you->mon, STAT_AGILITY, you->cls->agility_up, true);
    add_stat(you->mon, STAT_INTELLIGENCE, you->cls->intelligence_up, true);
    add_stat(you->mon, STAT_SPIRIT, you->cls->spirit_up, true);
    add_stat(you->mon, STAT_STAMINA, you->cls->stamina_up, true);
}

/**
 * Prompt player to confirm their character
 */
void confirm_character(void)
{
    term_clear();

    print_picked();

    print_options(CONFIRM, 0);

   do
   {
       switch(get_key())
       {
           case KEYCODE_y:
               _apply_stats();
               return;
           case KEYCODE_n:
               return;
           case KEYCODE_q:
               do_quit(); return;
       }
   }
   while(true);
}

/**
 * Goes through the character creation process.
 *
 * Should only be called once during a new game creation
 */
void do_char_creation(void)
{
    struct passwd* pd = getpwuid(getuid());
    if(pd == NULL)
        you->name = "unknown";
    else
        you->name = pd->pw_name;

    //pick_faction(you);

    pick_class();

    pick_race();

    confirm_character();
}
