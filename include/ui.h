#ifndef UI_H
#define UI_H

struct UIList
{
    void* head;
    int   count;
    void* current_selection;
};

void display_main_screen(void);
void display_char_status(void);
void display_char_info_screen(void);
void display_char_inventory(struct UIList* inv_list);

#endif
