#include "ui_stats.h"

#include "globals.h"
#include "input_keycodes.h"
#include "monster.h"
#include "mon_stats.h"
#include "player.h"
#include "player_class.h"
#include "term.h"

#include <stddef.h>

enum CharacterScreenCommand
{
    CHARACTER_SCREEN_COMMAND_QUIT = KEYCODE_q
};

void _display_stats(void)
{
    term_draw_ftext(2, 0, NULL, NULL, 0, "Name: %s", you->name);
    term_draw_ftext(1, 1, NULL, NULL, 0, "Class: %s", you->cls->name);

    term_draw_ftext(1, screen_rows-1, NULL, NULL, 0, "%c = close character screen", CHARACTER_SCREEN_COMMAND_QUIT);
}

bool character_screen_handler(void)
{
    enum CharacterScreenCommand in;
    do
    {
        term_clear();
        _display_stats();
        term_refresh();
        in = get_key();
    }
    while(in != CHARACTER_SCREEN_COMMAND_QUIT);

    term_clear();

    return false;
}
