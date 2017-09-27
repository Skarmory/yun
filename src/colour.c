#include <ncurses.h>
#include "colour.h"

void init_colours(void)
{
    start_color();

    init_color(COLOR_ORANGE, 1000, 490, 40);
    init_color(COLOR_PINK, 960, 550, 730);
    init_color(COLOR_BROWN, 700, 500, 400);
    init_color(COLOR_PURPLE, 580, 510, 790);
    init_color(COLOR_BLUE, 0, 440, 870);
    init_color(COLOR_WHITE, 1000, 1000, 1000);
    init_color(COLOR_CYAN, 410, 800, 940);
    init_color(COLOR_GREEN, 670, 830, 450);
    init_color(COLOR_YELLOW, 1000, 960, 410);
    init_color(COLOR_RED, 1000, 0, 0);

    init_pair(CP_BLACK_BLACK,   COLOR_BLACK,   COLOR_BLACK);
    init_pair(CP_RED_BLACK,     COLOR_RED,     COLOR_BLACK);
    init_pair(CP_GREEN_BLACK,   COLOR_GREEN,   COLOR_BLACK);
    init_pair(CP_YELLOW_BLACK,  COLOR_YELLOW,   COLOR_BLACK);
    init_pair(CP_BLUE_BLACK,    COLOR_BLUE,  COLOR_BLACK);
    init_pair(CP_MAGENTA_BLACK, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(CP_CYAN_BLACK,    COLOR_CYAN,    COLOR_BLACK);
    init_pair(CP_WHITE_BLACK,   COLOR_WHITE,   COLOR_BLACK);
    init_pair(CP_ORANGE_BLACK,  COLOR_ORANGE,  COLOR_BLACK);
    init_pair(CP_PINK_BLACK,    COLOR_PINK,    COLOR_BLACK);
    init_pair(CP_BROWN_BLACK,   COLOR_BROWN,   COLOR_BLACK);
    init_pair(CP_PURPLE_BLACK,  COLOR_PURPLE,  COLOR_BLACK);
}
