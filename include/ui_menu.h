#ifndef YUN_MENU_H
#define YUN_MENU_H

#define PICK_CLASS 0
#define CONFIRM    1

void print_picked(void);
void print_options(int what, short mask);

void pick_class(void);

void confirm_character(void);
void do_char_creation(void);

#endif
