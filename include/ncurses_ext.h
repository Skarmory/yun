#ifndef NAXX_NCURSES_EXT
#define NAXX_NCURSES_EXT

void draw_symbol(int x, int y, char sym, int colpair, int attr);
void mvprintw_xy(int x, int y, char* str, ...);
void mvprintwn_xy(int count, int x, int y, const char* str, ...);
void mvprintwa_xy(int x, int y, int attr, const char* str, ...);

#endif
