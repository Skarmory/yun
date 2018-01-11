// Character creation menu logic

#ifndef MENU_H
#define MENU_H

#define PICK_CLASS 0
#define PICK_RACE  1
#define PICK_SPEC  2
#define CONFIRM    3

void print_picked(void);
void print_options(int what, short mask);

void pick_race(void);
void pick_class(void);

void confirm_charater(void);
void do_char_creation(void);

#endif
