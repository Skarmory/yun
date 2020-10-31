#include "player_class.h"

#include "colour.h"
#include "mon_stats.h"

#define CLS(name, pl, mhk) \
    {\
        name, pl, mhk\
    }

/**
 * Array of all the player class types
 */
struct Class g_classes[] = 
{
    CLS("fighter", "fighters", 'f')
};

int g_classes_count = 1;

/**
 * Return the colour associated with given class based on how WoW addons defined them
 */
int get_class_colour(const struct Class* cls)
{
    if(!cls)
        return CLR_DEFAULT;

    return CLR_WHITE;
} 

/**
 * Return class index into class array by given class' unique char
 */
int get_class_idx(char cls)
{
    switch(cls)
    {
        case 'f':
            return CLS_FIGHTER_IDX;
        default:
            return -1;
    }
}
