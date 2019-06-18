#include "colour.h"
#include "globals.h"
#include "init.h"
#include "input.h"
#include "log.h"
#include "map.h"
#include "map_gen.h"
#include "map_room.h"
#include "message.h"
#include "monster.h"
#include "mon_ai.h"
#include "mon_type.h"
#include "obj_armour.h"
#include "obj_weapon.h"
#include "player.h"
#include "player_class.h"
#include "player_faction.h"
#include "player_race.h"
#include "symbol.h"
#include "term.h"
#include "ui.h"
#include "ui_menu.h"
#include "util.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define SIZE(x) sizeof(x)/sizeof(x[0])

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
                    term_draw_symbol(col, row, NULL, NULL, 0, tmp[j]);
                    ++col;
                }

                ++i;
            }

            term_draw_symbol(col, row, NULL, NULL, 0, buf[i]);
            ++col;
        }

        ++row;
    }

    fclose(intro);

    term_refresh();
    term_getch();
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
    if(!init_naxx())
    {
        printf("Initialisation failed. Check debug log.");
        return -1;
    }

    // some intro text
    term_draw_text((screen_cols/2) - 7, screen_rows/2, NULL, NULL, 0, "Welcome to Naxx");
    term_refresh();
    term_getch();

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

    startx = random_int(room->x + 1, room->x + room->w - 2);
    starty = random_int(room->y + 1, room->y + room->h - 2);

    // Add pile of testing longswords at random location in player start room
    struct WeaponBase* longsword_base = weapon_base_lookup_by_name("longsword");
    for(int i = 0; i < 5; ++i)
    {
        struct Weapon* longsword = weapon_new(longsword_base);
        loc_add_obj(&cmap->locs[startx][starty], longsword->obj);
    }

    struct Armour* armour;
    int rx0 = room->x+1;
    int rx1 = rx0 + room->w - 3;
    int ry0 = room->y+1;
    int ry1 = ry0 + room->h - 3;

    armour = armour_new(armour_base_lookup_by_name("magister's crown"));
    loc_add_obj(&cmap->locs[random_int(rx0, rx1)][random_int(ry0, ry1)], armour->obj);
    armour = armour_new(armour_base_lookup_by_name("magister's mantle"));
    loc_add_obj(&cmap->locs[random_int(rx0, rx1)][random_int(ry0, ry1)], armour->obj);
    armour = armour_new(armour_base_lookup_by_name("magister's robe"));
    loc_add_obj(&cmap->locs[random_int(rx0, rx1)][random_int(ry0, ry1)], armour->obj);
    armour = armour_new(armour_base_lookup_by_name("magister's gloves"));
    loc_add_obj(&cmap->locs[random_int(rx0, rx1)][random_int(ry0, ry1)], armour->obj);
    armour = armour_new(armour_base_lookup_by_name("magister's leggings"));
    loc_add_obj(&cmap->locs[random_int(rx0, rx1)][random_int(ry0, ry1)], armour->obj);
    armour = armour_new(armour_base_lookup_by_name("magister's boots"));
    loc_add_obj(&cmap->locs[random_int(rx0, rx1)][random_int(ry0, ry1)], armour->obj);

    armour = armour_new(armour_base_lookup_by_name("shadowcraft cap"));
    loc_add_obj(&cmap->locs[random_int(rx0, rx1)][random_int(ry0, ry1)], armour->obj);
    armour = armour_new(armour_base_lookup_by_name("shadowcraft spaulders"));
    loc_add_obj(&cmap->locs[random_int(rx0, rx1)][random_int(ry0, ry1)], armour->obj);
    armour = armour_new(armour_base_lookup_by_name("shadowcraft tunic"));
    loc_add_obj(&cmap->locs[random_int(rx0, rx1)][random_int(ry0, ry1)], armour->obj);
    armour = armour_new(armour_base_lookup_by_name("shadowcraft gloves"));
    loc_add_obj(&cmap->locs[random_int(rx0, rx1)][random_int(ry0, ry1)], armour->obj);
    armour = armour_new(armour_base_lookup_by_name("shadowcraft pants"));
    loc_add_obj(&cmap->locs[random_int(rx0, rx1)][random_int(ry0, ry1)], armour->obj);
    armour = armour_new(armour_base_lookup_by_name("shadowcraft boots"));
    loc_add_obj(&cmap->locs[random_int(rx0, rx1)][random_int(ry0, ry1)], armour->obj);

    armour = armour_new(armour_base_lookup_by_name("beaststalker's cap"));
    loc_add_obj(&cmap->locs[random_int(rx0, rx1)][random_int(ry0, ry1)], armour->obj);
    armour = armour_new(armour_base_lookup_by_name("beaststalker's spaulders"));
    loc_add_obj(&cmap->locs[random_int(rx0, rx1)][random_int(ry0, ry1)], armour->obj);
    armour = armour_new(armour_base_lookup_by_name("beaststalker's tunic"));
    loc_add_obj(&cmap->locs[random_int(rx0, rx1)][random_int(ry0, ry1)], armour->obj);
    armour = armour_new(armour_base_lookup_by_name("beaststalker's gloves"));
    loc_add_obj(&cmap->locs[random_int(rx0, rx1)][random_int(ry0, ry1)], armour->obj);
    armour = armour_new(armour_base_lookup_by_name("beaststalker's pants"));
    loc_add_obj(&cmap->locs[random_int(rx0, rx1)][random_int(ry0, ry1)], armour->obj);
    armour = armour_new(armour_base_lookup_by_name("beaststalker's boots"));
    loc_add_obj(&cmap->locs[random_int(rx0, rx1)][random_int(ry0, ry1)], armour->obj);

    armour = armour_new(armour_base_lookup_by_name("helm of valour"));
    loc_add_obj(&cmap->locs[random_int(rx0, rx1)][random_int(ry0, ry1)], armour->obj);
    armour = armour_new(armour_base_lookup_by_name("spaulders of valour"));
    loc_add_obj(&cmap->locs[random_int(rx0, rx1)][random_int(ry0, ry1)], armour->obj);
    armour = armour_new(armour_base_lookup_by_name("breastplate of valour"));
    loc_add_obj(&cmap->locs[random_int(rx0, rx1)][random_int(ry0, ry1)], armour->obj);
    armour = armour_new(armour_base_lookup_by_name("gauntlets of valour"));
    loc_add_obj(&cmap->locs[random_int(rx0, rx1)][random_int(ry0, ry1)], armour->obj);
    armour = armour_new(armour_base_lookup_by_name("legplates of valour"));
    loc_add_obj(&cmap->locs[random_int(rx0, rx1)][random_int(ry0, ry1)], armour->obj);
    armour = armour_new(armour_base_lookup_by_name("boots of valour"));
    loc_add_obj(&cmap->locs[random_int(rx0, rx1)][random_int(ry0, ry1)], armour->obj);

    struct MonType* ghoul_type = mon_type_lookup_by_name("ghoul");
    for(int i = 1; i < cmap->room_count - 1; i++)
    {
        room = cmap->rooms[i];
        startx = random_int(room->x + 1, room->x + room->w - 2);
        starty = random_int(room->y + 1, room->y + room->h - 2);

        struct Mon* ghoul = mon_new(ghoul_type, startx, starty);
        map_add_mon(cmap, ghoul);
    }
    // ---------- DEBUG CODE END ----------

    main_loop();

    do_quit();
}
