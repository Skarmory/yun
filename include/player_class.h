#ifndef YUN_CLASS_H
#define YUN_CLASS_H

#include "defs.h"

#define CLS_FIGHTER 0x000000001

#define CLS_INVALID_IDX (-1)
#define CLS_FIGHTER_IDX 0

/**
 * Struct containing data about player's profession
 */
struct Class 
{
    char* name;
    char* name_plural;
    char  hotkey;
};

extern int g_classes_count;
extern struct Class g_classes[];

int get_class_colour(const struct Class* cls);
int get_class_idx(char cls);

#endif
