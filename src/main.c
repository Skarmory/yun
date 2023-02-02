#include "gameplay.h"
#include "globals.h"
#include "init.h"
#include "map.h"
#include "map_cell.h"
#include "map_gen.h"
#include "map_room.h"
#include "message.h"
#include "monster.h"
#include "obj_weapon.h"
#include "player.h"
#include "spawn.h"
#include "spell.h"
#include "spell_effect.h"
#include "ui.h"
#include "ui_menu.h"
#include "util.h"

#include <scieppend/core/list.h>
#include <scieppend/core/log.h>
#include <scieppend/core/term.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void new_game(void);
void main_loop(void);

void new_game(void)
{
    do_char_creation();

    FILE* intro = fopen("intro.txt", "r");

    char buf[256];

    term_clear();

    int row = screen_rows/2 - 20;

    while(fgets(buf, sizeof(buf), intro) != NULL)
    {
        int col = screen_cols/2 - 50;

        for(int i = 0; i < (int)sizeof(buf) && buf[i] != '\0'; ++i)
        {
            if(buf[i] == '%')
            {
                char tmp[30];
                ++i;

                convert_arg(buf[i], tmp);

                for(int j = 0; j < (int)sizeof(tmp) && tmp[j] != '\0'; ++j)
                {
                    term_draw_symbol(col, row, COL(CLR_DEFAULT), COL(CLR_DEFAULT), 0, tmp[j]);
                    ++col;
                }

                ++i;
            }

            term_draw_symbol(col, row, COL(CLR_DEFAULT), COL(CLR_DEFAULT), 0, buf[i]);
            ++col;
        }

        ++row;
    }

    fclose(intro);

    term_refresh();
    term_wait_on_input();
    term_clear();

    current_turn_number = 0;
}

void main_loop(void)
{
    // Basic implementation for debugging

    display_main_screen();

    do
    {
        current_turn_number++;

        // Reset path generation info
        current_path_gen_id = -1;

        gameplay_turn();
        update_mons();

        clear_msgs();
        flush_msg_buffer();
        display_main_screen();
        term_refresh();
    }
    while(true);
}

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    if(!init_yun())
    {
        printf("Initialisation failed. Check debug log.");
        return -1;
    }

#ifdef DEBUG
    log_msg(LOG_DEBUG, "yun initialised with params:");
    for(int i = 0; i < argc; ++i)
    {
        log_format_msg(LOG_DEBUG, "\t%s", argv[i]);
    }
#endif

    // some intro text
    term_draw_text((screen_cols/2) - 7, screen_rows/2, COL(CLR_DEFAULT), COL(CLR_DEFAULT), 0, "Welcome to Yun");
    term_refresh();
    term_wait_on_input();

    new_player();
    new_game();

    int map_width = 3;
    int map_height = 3;
    g_cmap = map_new(map_width, map_height);
    gen_map(g_cmap, MAPTYPE_DUNGEON);

    mon_set_stat(g_you->mon, STAT_TYPE_HP_MAX, 999);
    mon_set_stat(g_you->mon, STAT_TYPE_HP, 999);

    struct MapCell* test_cell = map_get_cell_by_map_coord(g_cmap, 1, 1);

    // --------- DEBUG CODE START ----------
    struct Room* room = test_cell->room_list.head->data;
    int startx = random_int(room->x + 1, room->x + room->w - 2);
    int starty = random_int(room->y + 1, room->y + room->h - 2);

    g_you->mon->x = startx;
    g_you->mon->y = starty;
    map_cell_add_mon(test_cell, g_you->mon);

    startx = random_int(room->x + 1, room->x + room->w - 2);
    starty = random_int(room->y + 1, room->y + room->h - 2);

    // Add pile of testing longswords at random location in player start room
    struct WeaponBase* longsword_base = weapon_base_look_up_by_id("lswd");
    for(int i = 0; i < 5; ++i)
    {
        struct Weapon* longsword = weapon_new(longsword_base);
        loc_add_obj(map_cell_get_location(test_cell, startx, starty), longsword->obj);
    }

    struct ListNode* node;
    list_for_each(&test_cell->room_list, node)
    {
        room = node->data;
        startx = random_int(room->x + 1, room->x + room->w - 2);
        starty = random_int(room->y + 1, room->y + room->h - 2);
        spawn_mon("ghul", startx, starty);
    }

    // ---------- DEBUG CODE END ----------

    main_loop();

    do_quit();
}
