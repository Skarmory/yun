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

#define CLS_DRUID_IDX   0
#define CLS_HUNTER_IDX  1
#define CLS_MAGE_IDX    2
#define CLS_ROGUE_IDX   3
#define CLS_PALADIN_IDX 4
#define CLS_PRIEST_IDX  5
#define CLS_SHAMAN_IDX  6
#define CLS_WARLOCK_IDX 7
#define CLS_WARRIOR_IDX 8

struct Class 
{
    char* name;
    char* name_plural;
    char hotkey;

    short allow_races;
    short selfmask;

    unsigned char strength_up, agility_up, intelligence_up, spirit_up, stamina_up;
};

extern struct Class classes[];

int get_class_colour(const struct Class* cls);

#endif
