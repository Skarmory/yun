#include <ncurses.h>
#include "colour.h"

void init_colours(void)
{
    start_color();

    init_color(CLR_ORANGE, 1000, 490, 40);
    init_color(CLR_PINK, 960, 550, 730);
    init_color(CLR_BROWN, 700, 500, 400);
    init_color(CLR_PURPLE, 580, 510, 790);
    init_color(CLR_BLUE, 0, 440, 870);
    init_color(CLR_WHITE, 1000, 1000, 1000);
    init_color(CLR_CYAN, 410, 800, 940);
    init_color(CLR_GREEN, 670, 830, 450);
    init_color(CLR_YELLOW, 1000, 960, 410);
    init_color(CLR_RED, 1000, 0, 0);

    init_pair(CP_BLACK_BLACK,   CLR_BLACK,   CLR_BLACK);
    init_pair(CP_RED_BLACK,     CLR_RED,     CLR_BLACK);
    init_pair(CP_GREEN_BLACK,   CLR_GREEN,   CLR_BLACK);
    init_pair(CP_YELLOW_BLACK,  CLR_YELLOW,   CLR_BLACK);
    init_pair(CP_BLUE_BLACK,    CLR_BLUE,  CLR_BLACK);
    init_pair(CP_MAGENTA_BLACK, CLR_MAGENTA, CLR_BLACK);
    init_pair(CP_CYAN_BLACK,    CLR_CYAN,    CLR_BLACK);
    init_pair(CP_WHITE_BLACK,   CLR_WHITE,   CLR_BLACK);
    init_pair(CP_ORANGE_BLACK,  CLR_ORANGE,  CLR_BLACK);
    init_pair(CP_PINK_BLACK,    CLR_PINK,    CLR_BLACK);
    init_pair(CP_BROWN_BLACK,   CLR_BROWN,   CLR_BLACK);
    init_pair(CP_PURPLE_BLACK,  CLR_PURPLE,  CLR_BLACK);
}
