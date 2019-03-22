#include "ui.h"

#include "class.h"
#include "colour.h"
#include "equip.h"
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


void draw_textbox(int x, int y, int w, int h, char* text)
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

char prompt_choice(const char* title, struct List* choices)
{
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

    last_option_id = option_id-1;

    char choice;
    do
    {
        choice = getch();
    }
    while(choice < 'a' || choice > last_option_id);

    return choice;
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
