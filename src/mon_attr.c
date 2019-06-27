#include "mon_attr.h"

#include "string.h"

enum MonAttrMoveFlag mon_attr_move_flag_from_string(const char* name)
{
    if(strcmp(name, "walks") == 0 || strcmp(name, "WALKS") == 0)
        return MONATTR_WALKS;

    if(strcmp(name, "flies") == 0 || strcmp(name, "FLIES") == 0)
        return MONATTR_FLIES;

    if(strcmp(name, "swims") == 0 || strcmp(name, "SWIMS") == 0)
        return MONATTR_SWIMS;

    return 0;
}

