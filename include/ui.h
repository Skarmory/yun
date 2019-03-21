#ifndef UI_H
#define UI_H

#include "list.h"

#include <stdbool.h>

#define g_option_name_max_size 32

struct Equipment;
struct Inventory;
struct Object;

struct UIOption
{
    char   option_name[g_option_name_max_size];
    struct ListEntry option_list_entry;
};

void draw_textbox(int x, int y, int w, int h, char* text);

bool prompt_yn(const char* msg);
int  prompt_choice(const char* title, struct List* choices);

void display_main_screen(void);
void display_char_status(void);
void display_char_info_screen(void);

#endif
