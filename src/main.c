#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <ncurses.h>
#include <time.h>

#include "player.h"
#include "colour.h"
#include "class.h"
#include "race.h"
#include "faction.h"
#include "map.h"
#include "montype.h"
#include "mon.h"
#include "input.h"
#include "menu.h"
#include "util.h"
#include "message.h"
#include "log.h"
#include "ui.h"

#define SIZE(x) sizeof(x)/sizeof(x[0])

void new_game(void);
void main_loop(void);

int rows, cols;

void new_game(void)
{
    do_char_creation();

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
    display_char_status();

    do
    {
        handle_input();
        clear_msgs();
        flush_msg_buffer();
        display_map();
        display_char_status();
    }
    while(true);
}

int main(int argc, char** argv)
{
    srand(time(NULL));

    // ncurses init funcs
    initscr();
    noecho();
    raw();
    curs_set(0);

    init_colours();

    attron(COLOR_PAIR(CLR_DEFAULT));
    getmaxyx(stdscr, rows, cols);

    // some intro text
    mvprintw(rows/2, cols/2 - 7, "Welcome to Naxx");
    refresh();
    getch();

    init_logs();

    new_player();
    new_game();

    init_map();
    gen_map();

    struct Room* room = cmap->rooms[0];
    int startx = random_int(room->x + 1, room->x + room->w - 2);
    int starty = random_int(room->y + 1, room->y + room->h - 2);

    you->mon->x = startx;
    you->mon->y = starty;
    add_mon(you->mon);

    for(int i = 1; i < cmap->room_count - 1; i++)
    {
        room = cmap->rooms[i];
        startx = random_int(room->x + 1, room->x + room->w - 2);
        starty = random_int(room->y + 1, room->y + room->h - 2);

        struct Mon* ghoul = new_mon(MT_GHOUL, startx, starty);
        add_mon(ghoul);
    }

    main_loop();

    do_quit();
}
