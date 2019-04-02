#ifndef NAXX_TERM_H
#define NAXX_TERM_H

#include <stdbool.h>

typedef unsigned int TextAttributeFlags;
typedef enum TextAttributeFlag
{
    A_NONE_BIT       = 0,
    A_BOLD_BIT       = 1 << 0,
    A_UNDERSCORE_BIT = 1 << 1,
    A_BLINK_BIT      = 1 << 2,
    A_REVERSE_BIT    = 1 << 3
} TextAttributeFlag;

typedef enum TextAttribute
{
    A_NONE       = 0,
    A_BOLD       = 1,
    A_UNDERSCORE = 4,
    A_BLINK      = 5,
    A_REVERSE    = 7
} TextAttribute;

typedef struct Colour
{
    int r;
    int g;
    int b;
} Colour;

void term_init(void);
void term_uninit(void);

void term_get_wh(int* w, int* h);
void term_set_cursor(bool on);
void term_set_sigint_callback(void(*sig)(int));

void term_clear(void);
void term_clear_area(int x, int y, int w, int h);
void term_refresh(void);
char term_getch(void);

void term_draw_symbol(int x, int y, Colour* fg, Colour* bg, TextAttributeFlags ta_flags, char symbol);
void term_draw_text(int x, int y, Colour* fg, Colour* bg, TextAttributeFlags ta_flags, const char* text);
void term_draw_ftext(int x, int y, Colour* fg, Colour* bg, TextAttributeFlags ta_flags, const char* format, ...);
void term_draw_fntext(int count, int x, int y, Colour* fg, Colour* bg, TextAttributeFlags ta_flags, const char* format, ...);
void term_draw_area(int x, int y, int w, int h, Colour* fg, Colour* bg, TextAttributeFlags ta_flags, char symbol);

#endif
