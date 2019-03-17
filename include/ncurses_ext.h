#ifndef NCURSES_EXT
#define NCURSES_EXT

void draw_symbol(int x, int y, char sym, int colpair, int attr);
void mvprintw_xy(int x, int y, char* str, ...);
void mvprintwn_xy(int count, int x, int y, char* str, ...);
void mvprintwa_xy(int x, int y, int attr, char* str, ...);

#endif
