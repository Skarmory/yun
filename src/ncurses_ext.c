#include "ncurses_ext.h"
#include "log.h"

void mvprintw_xy(int x, int y, char* str, ...)
{
    va_list args;
    va_start(args, str);

    char tmp[256];
    vsprintf(tmp, str, args);
    mvprintw(y, x, tmp);

    va_end(args);
}

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
