#ifndef YUN_COMPONENTS_MOVEMENT_COMPONENT_H
#define YUN_COMPONENTS_MOVEMENT_COMPONENT_H

#include "mon_attr.h"

#include <scieppend/core/component.h>

COMPONENT_TYPE_DECL(MovementComponent)
{
    MonAttrMoveFlags move_flags;
};

#endif
