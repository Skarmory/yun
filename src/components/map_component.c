#include "components/map_component.h"

#include "map_cell.h"

#include <scieppend/core/event.h>
#include <stddef.h>

void map_component_init(struct MapComponent* map, int width, int height)
{
    map->width = width;
    map->height = height;
    array_init(&map->cells, sizeof(struct MapCell), (width * height), NULL, NULL);
}

void map_component_uninit(struct MapComponent* map)
{
    array_uninit(&map->cells);
}
