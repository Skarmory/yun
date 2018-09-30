#include "ncurses_ext.h"

#include "log.h"

#include <ncurses.h>

/**
 * Draws a symbol with given colours at given location
 */
void draw_symbol(int x, int y, char sym, int colpair, int attr)
{
    attron(attr);
    attron(COLOR_PAIR(colpair));
    mvaddch(y, x, sym);
    attroff(COLOR_PAIR(colpair));
    attroff(attr);
}

/**
 * Prints a formatted string given (x, y) rather than (y, x), which is more common notation
 */
void mvprintw_xy(int x, int y, char* str, ...)
{
    va_list args;
    va_start(args, str);

    char tmp[256];
    vsnprintf(tmp, 256, str, args);
    mvprintw(y, x, tmp);

    va_end(args);
}

/**
 * Prints a formatted string given (x, y) that had a particular ncurses attribute turned on
 */
void mvprintwa(int x, int y, int attr, char* str, ...)
{
    va_list args;
    va_start(args, str);

    char tmp[256];
    vsprintf(tmp, str, args);
    attron(attr);
    mvprintw(y, x, tmp);
    attroff(attr);

    va_end(args);
}
