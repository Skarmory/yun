#include "ui_stats.h"

#include "globals.h"
#include "monster.h"
#include "mon_stats.h"
#include "player.h"
#include "player_class.h"

#include <scieppend/core/colour.h>
#include <scieppend/core/input_keycodes.h>
#include <scieppend/core/term.h>

#include <stddef.h>

enum CharacterScreenCommand
{
    CHARACTER_SCREEN_COMMAND_QUIT = KEYCODE_q
};

void _display_stats(void)
{
    term_draw_ftext(2, 0, COL(CLR_DEFAULT), COL(CLR_DEFAULT), 0, "Name: %s", g_you->name);
    term_draw_ftext(1, 1, COL(CLR_DEFAULT), COL(CLR_DEFAULT), 0, "Class: %s", g_you->cls->name);

    term_draw_ftext(1, screen_rows-1, COL(CLR_DEFAULT), COL(CLR_DEFAULT), 0, "%c = close character screen", CHARACTER_SCREEN_COMMAND_QUIT);
}

bool character_screen_handler(void)
{
    enum CharacterScreenCommand in;
    do
    {
        term_clear();
        _display_stats();
        term_refresh();
        in = (enum CharacterScreenCommand)get_key();
    }
    while(in != CHARACTER_SCREEN_COMMAND_QUIT);

    term_clear();

    return false;
}
