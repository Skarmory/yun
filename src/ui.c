#include "ui.h"

#include "class.h"
#include "colour.h"
#include "globals.h"
#include "inventory.h"
#include "log.h"
#include "map.h"
#include "message.h"
#include "mon.h"
#include "ncurses_ext.h"
#include "object.h"
#include "player.h"
#include "race.h"
#include "stats.h"
#include "util.h"

#include <ncurses.h>
#include <stdio.h>
#include <string.h>

#define STATUS_X 0
#define STATUS_Y 45
#define STATUS_W 80

#define YES 'y'
#define NO  'n'

static void _textbox(int x, int y, int w, int h, char* text)
{
    int stridx   = 0;
    int textlen  = strlen(text);

    // Print text
    while(stridx < textlen)
    {
        int chars = w;
        if((stridx + w) < textlen)
        {
            chars = strrfindi(text, ' ', stridx + w);

            if(chars == -1)
                chars = w;
            else
                chars -= stridx;
        }

        mvprintwn_xy(chars, x, y++, text + stridx);
        stridx += chars;
    }
}

bool prompt_yn(const char* msg)
{
    clear_msgs();
    display_fmsg_nolog("%s [yn] (n)", msg);
    flush_msg_buffer();

    bool decision = getch() == YES;

    display_fmsg_log("%s [yn] (n) %c", msg, decision ? YES : NO);
    flush_msg_buffer();
    return decision;
}

int prompt_choice(const char* title, struct List* choices)
{
    log_msg(DEBUG, "prompt_choice");
    struct UIOption* option = list_head(choices, struct UIOption, option_list_entry);
    char option_id = 'a';
    char last_option_id;

    int x = (screen_cols / 2) - (g_option_name_max_size / 2);
    int y = (screen_rows / 2) - (choices->count / 2);

    mvprintwa_xy(x, y++, A_BOLD, "%s", title);

    while(option && option_id <= 'z')
    {
        mvprintw_xy(x, y++, "%c - %s", option_id, option->option_name);

        ++option_id;
        option = list_next(option, struct UIOption, option_list_entry);
    }

    last_option_id = option_id;

    char choice;
    do
    {
        choice = getch();
    }
    while(choice < 'a' || choice > last_option_id);

    return choice - 'a';
}

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

    mvprintwa_xy(1, 5, A_BOLD, "Strength       Agility        Intelligence   Spirit         Stamina");
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
    mvprintwa_xy(1, y, A_BOLD, "Inventory");
    y += 2;

    struct Object* obj = list_head(inv_list->head, struct Object, obj_list_entry);

    while(obj && y < displayable_rows)
    {
        if(obj == inv_list->current_selection)
        {
            mvprintwa_xy(1, y++, COLOR_PAIR(30), "%s", obj->name);
        }
        else
        {
            mvprintw_xy(1, y++, "%s", obj->name);
        }

        obj = list_next(obj, struct Object, obj_list_entry);
    }

    struct Object* selected_obj = (struct Object*)inv_list->current_selection;
    if(selected_obj)
    {
        int x_off = 32;
        const int desc_width = 64;

        y = 0;
        mvprintwa_xy(x_off, y, A_BOLD, "Description");
        y+=2;

        _textbox(x_off, y, desc_width, 0, selected_obj->desc);
    }

    mvprintw_xy(1, screen_rows-1, "q: close inventory / d: drop object");
}
