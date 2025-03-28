#ifndef YUN_COMPONENTS_POSITION_COMPONENT_H
#define YUN_COMPONENTS_POSITION_COMPONENT_H

#include <scieppend/core/component.h>

COMPONENT_TYPE_DECL(PositionComponent)
{
    int x;
    int y;
    int z;
};

void add_position_component(EntityHandle e_h, const int x, const int y, const int z);
void position_component_set_xyz(EntityHandle e_h, const int x, const int y, const int z);

#endif
