#ifndef NAXX_MON_ATTR_H
#define NAXX_MON_ATTR_H

#include "bit_flags.h"

enum MonAttrMoveFlag
{
    MONATTR_WALKS = BIT_FLAG(0),
    MONATTR_FLIES = BIT_FLAG(1),
    MONATTR_SWIMS = BIT_FLAG(2)
};
typedef unsigned int MonAttrMoveFlags;

enum MonAttrSenseFlag
{
    MON_ATTR_SENSE_SIGHT   = BIT_FLAG(0),
    MON_ATTR_SENSE_HEARING = BIT_FLAG(1),
    MON_ATTR_SENSE_SMELL   = BIT_FLAG(2)
};
typedef unsigned int MonAttrSenseFlags;

enum MonAttrMoveFlag  mon_attr_move_flag_from_string(const char* name);
enum MonAttrSenseFlag mon_attr_sense_flag_from_string(const char* name);

#endif
