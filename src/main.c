#include "class.h"
#include "colour.h"
#include "faction.h"
#include "globals.h"
#include "input.h"
#include "log.h"
#include "map.h"
#include "mapgen.h"
#include "menu.h"
#include "message.h"
#include "mon.h"
#include "montype.h"
#include "mon_ai.h"
#include "player.h"
#include "race.h"
#include "room.h"
#include "ui.h"
#include "util.h"
#include "weapon.h"

#include <ncurses.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE(x) sizeof(x)/sizeof(x[0])

void new_game(void);
void main_loop(void);

void new_game(void)
{
    do_char_creation();

    FILE* intro = fopen("intro.txt", "r");

    char buf[256];

    clear();

    int row = screen_rows/2 - 20;

    while(fgets(buf, sizeof(buf), intro) != NULL)
    {
        int col = screen_cols/2 - 38;

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

    current_turn_number = 0;
}

void main_loop(void)
{
    // Basic implementation for debugging

    display_map();
    display_char_status();

    do
    {
        current_turn_number++;

        // Reset path generation info
        current_path_gen_id = -1;

        handle_input();
        display_main_screen();

        update_mons();

        clear_msgs();
        flush_msg_buffer();

        display_main_screen();
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
    getmaxyx(stdscr, screen_rows, screen_cols);

    // some intro text
    mvprintw(screen_rows/2, screen_cols/2 - 7, "Welcome to Naxx");
    refresh();
    getch();

    init_logs();
    init_msgs();
    init_montypes();
    cmap = new_map();

    new_player();
    new_game();

    gen_map(cmap);

    // --------- DEBUG CODE START ----------
    struct Room* room = cmap->rooms[0];
    int startx = random_int(room->x + 1, room->x + room->w - 2);
    int starty = random_int(room->y + 1, room->y + room->h - 2);

    you->mon->x = startx;
    you->mon->y = starty;
    map_add_mon(cmap, you->mon);

    // Testing longsword
    struct Weapon* longsword = new_weapon(W_LONGSWORD);
    startx = random_int(room->x + 1, room->x + room->w - 2);
    starty = random_int(room->y + 1, room->y + room->h - 2);
    loc_add_obj(&cmap->locs[startx][starty], longsword->obj);
    //cmap->locs[startx][starty].objects = longsword->obj;

    for(int i = 1; i < cmap->room_count - 1; i++)
    {
        room = cmap->rooms[i];
        startx = random_int(room->x + 1, room->x + room->w - 2);
        starty = random_int(room->y + 1, room->y + room->h - 2);

        struct Mon* ghoul = new_mon(MT_GHOUL, startx, starty);
        map_add_mon(cmap, ghoul);
    }
    // ---------- DEBUG CODE END ----------

    main_loop();

    do_quit();
}
