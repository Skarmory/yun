#include "ui.h"

#include "colour.h"
#include "globals.h"
#include "log.h"
#include "map.h"
#include "message.h"
#include "monster.h"
#include "mon_equip.h"
#include "mon_inventory.h"
#include "mon_stats.h"
#include "ncurses_ext.h"
#include "object.h"
#include "player.h"
#include "player_class.h"
#include "player_race.h"
#include "util.h"

#include <ncurses.h>
#include <stdio.h>
#include <string.h>

#define STATUS_X 0
#define STATUS_Y 45
#define STATUS_W 80

#define YES 'y'
#define NO  'n'


void draw_textbox(int x, int y, int w, int h, const char* text)
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

void draw_textbox_border(int x, int y, int w, int h, const char* text)
{
    int _w = w + 4;
    int _h = h + 4;

    for(int _x = x; _x < (x+_w-1); ++_x)
    {
        draw_symbol(_x, y, '-', 0, 0);
        draw_symbol(_x, (y+_h-1), '-', 0, 0);
    }

    for(int _y = y; _y < (y+_h-1); ++_y)
    {
        draw_symbol(x, _y, '|', 0, 0);
        draw_symbol((x+_w-1), _y, '|', 0, 0);
    }

    draw_symbol(x, y, '+', 0, 0);
    draw_symbol(x+_w-1, y+_h-1, '+', 0, 0);
    draw_symbol(x+_w-1, y, '+', 0, 0);
    draw_symbol(x, y+_h-1, '+', 0, 0);

    draw_textbox(x+2, y+2, _w-2, _h-2, text);
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

char prompt_choice(const char* title, char** choices, int length)
{
    char last_option_id = 'a' + length - 1;

    int x = (screen_cols / 2) - (g_option_name_max_size / 2);
    int y = (screen_rows / 2) - (length / 2);

    mvprintwa_xy(x, y++, A_BOLD, "%s", title);

    for(int option_id = 0; option_id < length; ++option_id)
    {
        mvprintw_xy(x, y++, "%c - %s", (char)('a' + option_id), choices[option_id]);
    }

    char choice;
    do
    {
        choice = getch();
    }
    while(choice != g_key_escape && (choice < 'a' || choice > last_option_id));

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
