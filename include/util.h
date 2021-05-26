#ifndef YUN_UTIL_H
#define YUN_UTIL_H

#include <stddef.h>

void convert_arg(char c, char* buf);

int strrfindi(const char* haystack, const char needle, size_t start);

void do_quit(void);

void sigint_handler(int _);

int random_int(int hi, int lo);

int roll_dice(int dice_count, int dice_sides);

int roll_d100(void);

float roll_d100f(void);

float log_base(int x, int base);

int clamp(int val, int low, int high);

#define minu(a, b) ( (a) > (b) ? b : a )
#define maxu(a, b) ( (a) > (b) ? a : b )

#endif
