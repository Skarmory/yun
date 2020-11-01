#include "ui_menu.h"

#include "colour.h"
#include "input_keycodes.h"
#include "log.h"
#include "monster.h"
#include "mon_stats.h"
#include "mon_type.h"
#include "player.h"
#include "player_class.h"
#include "player_faction.h"
#include "ui.h"
#include "util.h"

#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * Print out the character creation choices made so far
 */
void print_picked(void)
{
    int col;
    term_draw_ftext(1, 2, NULL, NULL, 0, "   name: %s", !g_you->name ? "unknown" : g_you->name);

    col = get_class_colour(g_you->cls);
    term_draw_text(2, 2, NULL, NULL, 0, "  class: ");
    term_draw_ftext(11, 2, COL(col), NULL, 0, "%s", !g_you->cls ? "not chosen" : g_you->cls->name);
}

/**
 * Print character creation current choice to make
 */
void print_options(int what, short mask)
{
    (void)mask;
    int menu_col = 60, menu_row = 1;

    switch(what)
    {
        case PICK_CLASS:
        {
            term_draw_text(menu_col, menu_row, NULL, NULL, 0, "Choose g_your class");
            menu_row += 2;
            for(int i = 0; i < g_classes_count; i++, menu_row++)
            {
                int col = get_class_colour(&g_classes[i]);
                term_draw_ftext(menu_col, menu_row, col != CLR_DEFAULT ? COL(col) : NULL, NULL, 0, "%c - %s", g_classes[i].hotkey, g_classes[i].name);
            }
            break;
        }

        case CONFIRM:
        {
            term_draw_text(menu_col, menu_row, NULL, NULL, 0, "Confirm this character and start a new game?");
            menu_row += 2;
            term_draw_text(menu_col, menu_row++, NULL, NULL, 0, "y - Start game");
            term_draw_text(menu_col, menu_row++, NULL, NULL, 0, "n - Choose again");
            break;
        }
    }

    menu_row++;
    term_draw_text(menu_col, menu_row, NULL, NULL, 0, "q - quit");

    term_refresh();
}

/**
 * Logic for picking a class
 *
 * Loops on blocking input until player makes a valid choice
 *
 * Set stats based on choice
 */
void pick_class(void)
{
    term_clear();

    print_picked();

    print_options(PICK_CLASS, 0);

    g_you->cls = malloc(sizeof(struct Class));

    bool picked = false;

    do
    {
        enum KeyCode choice = get_key();
        if(choice == KEYCODE_q)
        {
            do_quit();
            return;
        }

        int cls_idx = get_class_idx(choice);
        if(cls_idx == -1)
        {
            continue;
        }

        *(g_you->cls) = g_classes[cls_idx];
        picked = true;
    }
    while(!picked);

}

void _apply_stats(void)
{
    mon_set_stat(g_you->mon, STAT_TYPE_HP_MAX, g_you->mon->type->base_stats.hp_max);
    mon_set_stat(g_you->mon, STAT_TYPE_HP, g_you->mon->type->base_stats.hp_max);
}

enum ConfirmCharacterCommand
{
    CONFIRM_CHARACTER_YES  = YES,
    CONFIRM_CHARACTER_NO   = NO,
    CONFIRM_CHARACTER_QUIT = KEYCODE_q
};

/**
 * Prompt player to confirm their character
 */
void confirm_character(void)
{
    term_clear();

    print_picked();

    print_options(CONFIRM, 0);

   do
   {
       switch((enum ConfirmCharacterCommand)get_key())
       {
           case CONFIRM_CHARACTER_YES:
               _apply_stats();
               return;
           case CONFIRM_CHARACTER_NO:
               return;
           case CONFIRM_CHARACTER_QUIT:
               do_quit(); return;
       }
   }
   while(true);
}

/**
 * Goes through the character creation process.
 *
 * Should only be called once during a new game creation
 */
void do_char_creation(void)
{
    struct passwd* pd = getpwuid(getuid());
    if(pd == NULL)
    {
        g_you->name = "unknown";
    }
    else
    {
        g_you->name = pd->pw_name;
    }

    pick_class();

    confirm_character();
}
