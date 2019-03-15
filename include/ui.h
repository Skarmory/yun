#ifndef UI_H
#define UI_H

#include "list.h"

#include <stdbool.h>

struct UIList
{
    struct List* head;
    int          count;
    void*        current_selection;
};

bool prompt_yn(const char* msg);
void display_main_screen(void);
void display_char_status(void);
void display_char_info_screen(void);
void display_char_inventory(struct UIList* inv_list);

#endif
