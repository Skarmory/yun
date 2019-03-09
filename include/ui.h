#ifndef UI_H
#define UI_H

#include <stdbool.h>

struct UIList
{
    void* head;
    int   count;
    void* current_selection;
};

bool prompt_yn(const char* msg);
void display_main_screen(void);
void display_char_status(void);
void display_char_info_screen(void);
void display_char_inventory(struct UIList* inv_list);

#endif
