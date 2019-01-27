#ifndef UI_H
#define UI_H

#define UI_CLOSE 'q'

struct UIState
{
    int highlighted;
    void* selected_list;
    char input;
};

typedef void (*UIDrawFunc)(struct UIState*);

void interactive_screen(UIDrawFunc func, struct UIState* state);

void display_main_screen(void);
void display_char_status(void);
void display_char_info_screen(void);
void display_char_inventory(struct UIState* state);

#endif
