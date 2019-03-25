#ifndef NAXX_UI_H
#define NAXX_UI_H

#include "list.h"

#include <stdbool.h>

#define g_option_name_max_size 32

#define g_key_escape 27

struct Equipment;
struct Inventory;
struct Object;

struct UIOption
{
    char   option_name[g_option_name_max_size];
    struct ListEntry option_list_entry;
};

void draw_textbox(int x, int y, int w, int h, const char* text);

bool prompt_yn(const char* msg);
char prompt_choice_list(const char* title, struct List* choices);
char prompt_choice_array(const char* title, char** choices, int length);

void display_main_screen(void);
void display_char_status(void);
void display_char_info_screen(void);

#endif
