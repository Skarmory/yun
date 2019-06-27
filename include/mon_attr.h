#ifndef NAXX_MON_ATTR_H
#define NAXX_MON_ATTR_H

enum MonAttrMoveFlag
{
    MONATTR_WALKS = 1u,
    MONATTR_SWIMS = (1u << 2),
    MONATTR_FLIES = (1u << 1)
};

typedef unsigned int MonAttrMoveFlags;

enum MonAttrMoveFlag mon_attr_move_flag_from_string(const char* name);

#endif
