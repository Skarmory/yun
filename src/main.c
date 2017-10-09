#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <ncurses.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <pwd.h>

#include "player.h"
#include "colour.h"
#include "class.h"
#include "race.h"
#include "faction.h"
#include "map.h"
#include "montype.h"
#include "mon.h"

#define PICK_CLASS 0
#define PICK_RACE  1
#define PICK_SPEC  2
#define CONFIRM    3

#define SIZE(x) sizeof(x)/sizeof(x[0])

// Forward declarations
void do_quit(void);
void print_picked(void);
void print_options(int what, short mask);

void pick_race(void);
void pick_class(void);

void confirm_charater(void);
void do_char_creation(void);

void new_game(void);
void main_loop(void);

int rows, cols;
struct Player* you;

void do_quit(void)
{
    endwin();
    use_default_colors();
    puts("Your curiosity will be the death of you.");
    exit(0);
}

void print_picked(void)
{
    int col;
    mvprintw(1, 2, "   name: %s", !you->name ? "unknown" : you->name);

    mvprintw(2, 2, "  class: ");
    col = get_class_colour(you->cls);
    attron(COLOR_PAIR(col));
    mvprintw(2, 11, "%s", !you->cls ? "not chosen" : you->cls->name);
    attroff(COLOR_PAIR(col));

    col = you->race ? you->race->faction == FA_ALLIANCE ? CP_ALLIANCE : CP_HORDE : CP_DEFAULT;
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
                    int col = races[i].faction == FA_ALLIANCE ? CP_ALLIANCE : CP_HORDE;
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

    do
    {
        switch(getch())
        {
            case 'd':
                *(you->cls) = classes[0]; return;
            case 'h':
                *(you->cls) = classes[1]; return;
            case 'm':
                *(you->cls) = classes[2]; return;
            case 'r':
                *(you->cls) = classes[3]; return;
            case 'p':
                *(you->cls) = classes[4]; return;
            case 'i':
                *(you->cls) = classes[5]; return;
            case 's':
                *(you->cls) = classes[6]; return;
            case 'l':
                *(you->cls) = classes[7]; return;
            case 'w':
                *(you->cls) = classes[8]; return;
            case 'q':
                do_quit(); return;
        }
    }
    while(true);
}

void pick_race(void)
{
    clear();

    print_picked();

    short allowed = you->cls->allow_races & 0xffff;

    print_options(PICK_RACE, allowed);

    you->race = (struct Race*)malloc(sizeof(struct Race));

    do
    {
        switch(getch())
        {
            case 'h':
                *(you->race) = races[0]; you->faction = "alliance"; return;
            case 'd':
                *(you->race) = races[1]; you->faction = "alliance"; return;
            case 'e':
                *(you->race) = races[2]; you->faction = "alliance"; return;
            case 'g':
                *(you->race) = races[3]; you->faction = "alliance"; return;
            case 'o':
                *(you->race) = races[4]; you->faction = "horde"; return;
            case 'f':
                *(you->race) = races[5]; you->faction = "horde"; return;
            case 't':
                *(you->race) = races[6]; you->faction = "horde"; return;
            case 'l':
                *(you->race) = races[7]; you->faction = "horde"; return;
            case 'q':
                do_quit(); return;
        }
    }
    while(true);
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

void convert_arg(char c, char* buf)
{
    switch(c)
    {
        case 'C':
            strcpy(buf, you->cls->name_plural);
            return;
        case 'c':
            strcpy(buf, you->cls->name);
            return;
        case 'F':
            strcpy(buf, you->race->faction == FA_ALLIANCE ? "The Alliance" : "The Horde");
            return;
        case 'H':
            strcpy(buf, you->race->home);
            return;
        case 'L':
            strcpy(buf, you->race->leader);
            return;
    }
}

void new_game(void)
{
    FILE* intro = fopen("intro.txt", "r");

    char buf[256];

    clear();

    int row = rows/2 - 20;

    while(fgets(buf, sizeof(buf), intro) != NULL)
    {
        int col = cols/2 - 38;

        for(int i = 0; i < SIZE(buf) && buf[i] != '\0'; ++i)
        {
            if(buf[i] == '%')
            {
                char tmp[30];
                ++i;

                convert_arg(buf[i], tmp);

                for(int j = 0; j < SIZE(tmp) && tmp[j] != '\0'; ++j)
                {
                    mvaddch(row, col, tmp[j]);
                    ++col;
                }

                ++i;
            }

            mvaddch(row, col, buf[i]);
            ++col;
        }

        ++row;
    }

    fclose(intro);

    getch();
    clear();
}

void main_loop(void)
{
    // Basic implementation for debugging

    display_map();

    for(;;)
    {
        bool move = true;
        do
        {
            char ch;
            switch(ch = getch()) // get movement
            {
                case 'h':
                case 'j':
                case 'k':
                case 'l':
                    move = false;
                    you->mon->x += (ch == 'h' ? -1 : ch == 'l' ? 1 : 0);
                    you->mon->y += (ch == 'k' ? -1 : ch == 'j' ? 1 : 0);
                    break;
                case 'q':
                    do_quit();
                    break;
            }
        } while(move);

        display_map();
    }
}

int main(int argc, char** argv)
{
    // ncurses init funcs
    initscr();
    noecho();
    raw();

    init_colours();

    attrset(COLOR_PAIR(CP_DEFAULT));
    getmaxyx(stdscr, rows, cols);

    // some intro text
    mvprintw(rows/2, cols/2 - 7, "Welcome to Naxx");
    refresh();
    getch();

    you = (struct Player*)malloc(sizeof(struct Player));
    you->name = NULL;
    you->race = NULL;
    you->cls = NULL;

    you->mon = (struct Mon*) malloc(sizeof(struct Mon));
    you->mon->x = 5;
    you->mon->y = 5;

    you->mon->type = &mon_type[MT_PLAYER];

    do_char_creation();

    new_game();

    init_map();

    main_loop();

    do_quit();
}
