#include "player.h"

#include <ncurses.h>
#include <stdlib.h>

#include "ui.h"
#include "map.h"
#include "util.h"
#include "message.h"

struct Player* you;

void new_player(void)
{
    you = (struct Player*)malloc(sizeof(struct Player));
    you->mon = new_mon(MT_PLAYER, 0, 0);
    you->name = NULL;
    you->faction = NULL;
    you->cls = NULL;
    you->race = NULL;
}

void destroy_player(void)
{
    if(you != NULL)
    {
        you->faction = NULL;
        free(you->cls);
        free(you->race);
        if(you->mon != NULL)
            free(you->mon);
        free(you); 
    }
}

void player_chk_dead(void)
{
    if(mon_is_dead(you->mon))
    {
        clear_msgs();
        flush_msg_buffer();

        display_msg("You died...");

        clear_msgs();
        flush_msg_buffer();

        display_map();
        display_char_status();

        getch();
        do_quit();
    }
}
