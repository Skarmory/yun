#ifndef NAXX_UTIL_H
#define NAXX_UTIL_H

void convert_arg(char c, char* buf);

int strrfindi(const char* haystack, const char needle, int start);

void do_quit(void);

int random_int(int hi, int lo);

int roll_d100(void);

float roll_d100f(void);

float log_base(int x, int base);

#endif
