#ifndef CLASS_H
#define CLASS_H

#define CLS_DRUID   0x00000001
#define CLS_HUNTER  0x00000002
#define CLS_MAGE    0x00000004
#define CLS_ROGUE   0x00000008
#define CLS_PALADIN 0x00000010
#define CLS_PRIEST  0x00000020
#define CLS_SHAMAN  0x00000040
#define CLS_WARLOCK 0x00000080
#define CLS_WARRIOR 0x00000100 

struct Class 
{
    char* name;
    char hotkey;

    short allow_races;
    short selfmask;
};

int get_class_colour(const struct Class* cls);

#endif
