#ifndef YUN_COMPONENTS_MAP_COMPONENT_H
#define YUN_COMPONENTS_MAP_COMPONENT_H


#include <scieppend/core/array.h>
#include <scieppend/core/component.h>

COMPONENT_TYPE_DECL(MapComponent)
{
    int width;
    int height;
    struct Array cells;
};

void map_component_init(struct MapComponent* map, int width, int height);
void map_component_uninit(struct MapComponent* map);

#endif
