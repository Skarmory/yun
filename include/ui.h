#ifndef YUN_UI_H
#define YUN_UI_H

#include <scieppend/core/input_keycodes.h>
#include <scieppend/core/list.h>

#include <stdbool.h>

#define g_option_name_max_size 32

struct Colour;
struct Equipment;
struct Inventory;
struct Object;

enum YesNoCommand
{
    YES = KEYCODE_y,
    NO  = KEYCODE_n
};

void draw_textbox(int x, int y, int w, int h, struct Colour* fg, struct Colour* bg, const char* text);
void draw_textbox_border(int x, int y, int w, int h, struct Colour* fg, struct Colour* bg, const char* text);

bool prompt_yn(const char* msg);
char prompt_choice(const char* title, char** choices, int length);

void display_main_screen(void);
void display_char_status(void);
void display_char_info_screen(void);

#endif
