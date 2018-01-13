#include "ui.h"
#include "player.h"
#include <stdio.h>
#include <ncurses.h>

#define STATUS_X 0
#define STATUS_Y 45
#define STATUS_W 80

void display_char_status(void)
{
    char tmp[STATUS_W];
    sprintf(tmp, "%s\t", you->name);    
    sprintf(tmp, "%sHP: %d/%d\t", tmp, you->mon->hp, you->mon->max_hp);    
    sprintf(tmp, "%sStr:%d Agi:%d Int:%d Spi:%d Sta:%d", tmp, PSTAT(strength, strength), PSTAT(agility, agility), PSTAT(intelligence, intelligence), PSTAT(spirit, spirit), PSTAT(stamina, stamina));    

    mvprintw(STATUS_Y, STATUS_X, tmp); 
}
