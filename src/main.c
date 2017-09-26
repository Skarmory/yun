#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <pwd.h>
#include <stddef.h>

#include "player.h"

// Faction defines
#define FA_ALLIANCE 0x00000001
#define FA_HORDE    0x00000001

#define PICK_CLASS 0
#define PICK_RACE  1

#define SIZE(x) sizeof(x)/sizeof(x[0])

// Forward declarations
void do_quit(void);
void print_picked(struct Player* you);
void print_options(int what, short mask);

void pick_race(struct Player* you);
void pick_class(struct Player* you);

void do_char_creation(struct Player*);

void do_quit(void)
{
    endwin();
    puts("Your curiosity will be the death of you.");
    exit(0);
}

void print_picked(struct Player* you)
{
    mvprintw(1, 4, "name: %s", !you->name ? "unknown" : you->name);
    mvprintw(2, 3, "class: %s", !you->cls ? "not chosen" : you->cls->name);
    mvprintw(3, 4, "race: %s", !you->race ? "not chosen" : you->race->noun);
}

void print_options(int what, short mask)
{
    int menu_col = 60, menu_row = 1;
   
    switch(what)
    {
        case PICK_CLASS:
            mvprintw(menu_row, menu_col, "Choose your class");
            menu_row += 2;
            for(int i = 0; i < SIZE(classes); i++, menu_row++)
                mvprintw(menu_row, menu_col, "%c - %s", classes[i].hotkey, classes[i].name);

            break;

        case PICK_RACE:
            mvprintw(menu_row, menu_col, "Choose your race");
            menu_row += 2;
            for(int i = 0; i < SIZE(races); i++)
            {
                if(races[i].selfmask & mask)
                {
                    mvprintw(menu_row, menu_col, "%c - %s", races[i].hotkey, races[i].noun);
                    menu_row++;
                }
            }

            break;
    }

    menu_row++;
    mvprintw(menu_row, menu_col, "q - quit");
}

void pick_class(struct Player* you)
{
    clear();

    print_picked(you);
    
    print_options(PICK_CLASS, 0);
   
    you->cls = (struct Class*)malloc(sizeof(struct Class));

    do
    {
        switch(getch())
        {
            case 'h':
                *(you->cls) = classes[0]; break;
            case 'm':
                *(you->cls) = classes[1]; break;
            case 'r':
                *(you->cls) = classes[2]; break;
            case 'd':
                *(you->cls) = classes[3]; break;
            case 'p':
                *(you->cls) = classes[4]; break;
            case 's':
                *(you->cls) = classes[6]; break;
            case 'l':
                *(you->cls) = classes[5]; break;
            case 'w':
                *(you->cls) = classes[7]; break;
            case 'q':
                do_quit(); break;
        }
    } 
    while(!you->cls);
}

void pick_race(struct Player* you)
{
    clear();

    print_picked(you);

    short allowed = you->cls->allow_races & 0xffff;

    print_options(PICK_RACE, allowed);

    you->race = (struct Race*)malloc(sizeof(struct Race));

    do 
    {
        switch(getch())
        {
            case 'h':
                *(you->race) = races[0]; break;
            case 'd':
                *(you->race) = races[1]; break;
            case 'e':
                *(you->race) = races[2]; break;
            case 'g':
                *(you->race) = races[3]; break;
            case 'o':
                *(you->race) = races[4]; break;
            case 'f':
                *(you->race) = races[5]; break;
            case 't':
                *(you->race) = races[6]; break;
            case 'l':
                *(you->race) = races[7]; break;
            case 'q':
                do_quit(); break;
        }
    }
    while(!you->race);
}


void do_char_creation(struct Player* you)
{
    struct passwd* pd = getpwuid(getuid());
    if(pd == NULL)
        you->name = "unknown";
    else
        you->name = pd->pw_name;

    //pick_faction(you);

    pick_class(you);

    pick_race(you);

    clear();

    print_picked(you);

    getch();
}

int main(int argc, char** argv)
{
    // ncurses init funcs
    initscr();
    noecho();
    raw();

    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    // some intro text
    mvprintw(rows/2, cols/2 - 7, "Welcome to Naxx");
    refresh();
    getch();

    struct Player* you = (struct Player*)malloc(sizeof(struct Player));
    you->name = NULL;
    you->race = NULL;
    you->cls = NULL;

    do_char_creation(you);

    do_quit();
}
