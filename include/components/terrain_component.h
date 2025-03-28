#ifndef YUN_COMPONENTS_TERRAIN_COMPONENT_H
#define YUN_COMPONENTS_TERRAIN_COMPONENT_H

#include <scieppend/core/component.h>

COMPONENT_TYPE_DECL(TerrainComponent)
{
    int id;
};

void add_terrain_component(EntityHandle e_h, int terrain_id);

int terrain_component_get_id(EntityHandle e_h);
void terrain_component_set_id(EntityHandle e_h, int id);

#endif
