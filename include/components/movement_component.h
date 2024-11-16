#ifndef YUN_COMPONENTS_MOVEMENT_COMPONENT_H
#define YUN_COMPONENTS_MOVEMENT_COMPONENT_H

#include "components/component_decl.h"

#include "mon_attr.h"

DECL_COMPONENT(MovementComponent)
{
    MonAttrMoveFlags move_flags;
};

#endif
