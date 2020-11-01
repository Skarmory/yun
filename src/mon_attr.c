#include "mon_attr.h"

#include <string.h>

enum MonAttrMoveFlag mon_attr_move_flag_from_string(const char* name)
{
    if(strcmp(name, "walks") == 0 || strcmp(name, "WALKS") == 0)
    {
        return MONATTR_WALKS;
    }

    if(strcmp(name, "flies") == 0 || strcmp(name, "FLIES") == 0)
    {
        return MONATTR_FLIES;
    }

    if(strcmp(name, "swims") == 0 || strcmp(name, "SWIMS") == 0)
    {
        return MONATTR_SWIMS;
    }

    return 0;
}

enum MonAttrSenseFlag mon_attr_sense_flag_from_string(const char* name)
{
    if(strcmp(name, "sight") == 0 || strcmp(name, "SIGHT") == 0)
    {
        return MON_ATTR_SENSE_SIGHT;
    }

    if(strcmp(name, "hearing") == 0 || strcmp(name, "HEARING") == 0)
    {
        return MON_ATTR_SENSE_HEARING;
    }

    if(strcmp(name, "smell") == 0 || strcmp(name, "SMELL") == 0)
    {
        return MON_ATTR_SENSE_SMELL;
    }

    return 0;
}
