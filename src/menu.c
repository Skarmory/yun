#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <ncurses.h>
#include "menu.h"
#include "player.h"
#include "colour.h"
#include "util.h"
#include "stats.h"

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

void pick_class(void)
{
    clear();

    print_picked();

    print_options(PICK_CLASS, 0);

    you->cls = (struct Class*)malloc(sizeof(struct Class));

    bool picked = false;

    do
    {
        switch(getch())
        {
            case 'd':
                *(you->cls) = classes[CLS_DRUID_IDX]; picked = true; break;
            case 'h':
                *(you->cls) = classes[CLS_HUNTER_IDX]; picked = true; break;
            case 'm':
                *(you->cls) = classes[CLS_MAGE_IDX]; picked = true; break;
            case 'r':
                *(you->cls) = classes[CLS_ROGUE_IDX]; picked = true; break;
            case 'p':
                *(you->cls) = classes[CLS_PALADIN_IDX]; picked = true; break;
            case 'i':
                *(you->cls) = classes[CLS_PRIEST_IDX]; picked = true; break;
            case 's':
                *(you->cls) = classes[CLS_SHAMAN_IDX]; picked = true; break;
            case 'l':
                *(you->cls) = classes[CLS_WARLOCK_IDX]; picked = true; break;
            case 'w':
                *(you->cls) = classes[CLS_WARRIOR_IDX]; picked = true; break;
            case 'q':
                do_quit(); return;
        }
    }
    while(!picked);

    you->mon->stats.primary = you->cls->primary_stat;
    you->mon->stats.secondary = you->cls->secondary_stat;
    you->mon->stats.tertiary = you->cls->tertiary_stat;

    add_stat(you->mon, STRENGTH, you->cls->strength_up, true);
    add_stat(you->mon, AGILITY, you->cls->agility_up, true);
    add_stat(you->mon, INTELLIGENCE, you->cls->intelligence_up, true);
    add_stat(you->mon, SPIRIT, you->cls->spirit_up, true);
    add_stat(you->mon, STAMINA, you->cls->stamina_up, true);
}

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
            char* faction_name = get_faction_name(get_faction_by_race(choice));

            *(you->race) = races[race_idx];
            you->faction = faction_name;
            picked = true;
        }
    }
    while(!picked);

    add_stat(you->mon, STRENGTH, you->race->strength_up, true);
    add_stat(you->mon, AGILITY, you->race->agility_up, true);
    add_stat(you->mon, INTELLIGENCE, you->race->intelligence_up, true);
    add_stat(you->mon, SPIRIT, you->race->spirit_up, true);
    add_stat(you->mon, STAMINA, you->race->stamina_up, true);
}

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
               return;
           case 'n':
               return;
           case 'q':
               do_quit(); return;
       }
   }
   while(true);
}

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
