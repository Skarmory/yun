#ifndef YUN_COMPONENTS_MAP_COMPONENT_H
#define YUN_COMPONENTS_MAP_COMPONENT_H

#include "components/component_decl.h"

#include <scieppend/core/array.h>

DECL_COMPONENT(MapComponent)
{
    int width;
    int height;
    struct Array cells;
};

void map_component_init(struct MapComponent* map, int width, int height);
void map_component_uninit(struct MapComponent* map);

#endif
