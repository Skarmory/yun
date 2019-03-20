#include "menu.h"

#include "class.h"
#include "colour.h"
#include "faction.h"
#include "log.h"
#include "mon.h"
#include "montype.h"
#include "player.h"
#include "stats.h"
#include "race.h"
#include "util.h"

#include <ncurses.h>
#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * Print out the character creation choices made so far
 */
void print_picked(void)
{
    int col;
    mvprintw(1, 2, "   name: %s", !you->name ? "unknown" : you->name);

    mvprintw(2, 2, "  class: ");
    col = get_class_colour(you->cls);
    attron(COLOR_PAIR(col));
    mvprintw(2, 11, "%s", !you->cls ? "not chosen" : you->cls->name);
    attroff(COLOR_PAIR(col));

    col = you->race ? you->race->faction == FA_ALLIANCE ? CLR_ALLIANCE : CLR_HORDE : CLR_DEFAULT;
    mvprintw(3, 2, "   race: ", !you->race ? "not chosen" : you->race->noun);
    mvprintw(4, 2, "faction: ", !you->faction ? "not chosen" : you->faction);
    attron(COLOR_PAIR(col));
    mvprintw(3, 11, "%s", !you->race ? "not chosen" : you->race->noun);
    mvprintw(4, 11, "%s", !you->faction ? "not chosen" : you->faction);
    attroff(COLOR_PAIR(col));
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
            mvprintw(menu_row, menu_col, "Choose your class");
            menu_row += 2;
            for(int i = 0; i < 9; i++, menu_row++)
            {
                int col = get_class_colour(&classes[i]);
                attron(COLOR_PAIR(col));
                mvprintw(menu_row, menu_col, "%c - %s", classes[i].hotkey, classes[i].name);
                attroff(COLOR_PAIR(col));
            }

            break;

        case PICK_RACE:
            mvprintw(menu_row, menu_col, "Choose your race");
            menu_row += 2;
            for(int i = 0; i < 8; i++)
            {
                if(races[i].selfmask & mask)
                {
                    int col = races[i].faction == FA_ALLIANCE ? CLR_ALLIANCE : CLR_HORDE;
                    you->faction = factions[races[i].faction];
                    attron(COLOR_PAIR(col));

                    mvprintw(menu_row, menu_col, "%c - %s", races[i].hotkey, races[i].noun);
                    menu_row++;

                    attroff(COLOR_PAIR(col));
                }
            }

            break;

        case CONFIRM:
            mvprintw(menu_row, menu_col, "Confirm this character and start new game?");
            menu_row += 2;
            mvprintw(menu_row++, menu_col, "y - Start game");
            mvprintw(menu_row++, menu_col, "n - Choose again");
            break;
    }

    menu_row++;
    mvprintw(menu_row, menu_col, "q - quit");
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
    clear();

    print_picked();

    print_options(PICK_CLASS, 0);

    you->cls = (struct Class*)malloc(sizeof(struct Class));

    bool picked = false;

    do
    {
        char choice = getch();
        if(choice == 'q')
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
    clear();

    print_picked();

    short allowed = you->cls->allow_races & 0xffff;

    print_options(PICK_RACE, allowed);

    you->race = (struct Race*)malloc(sizeof(struct Race));

    bool picked = false;

    do
    {
        char choice = getch();
        if(choice == 'q')
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
    clear();

    print_picked();

    print_options(CONFIRM, 0);

   do
   {
       switch(getch())
       {
           case 'y':
               _apply_stats();
               return;
           case 'n':
               return;
           case 'q':
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
