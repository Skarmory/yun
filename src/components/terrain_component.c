#include "components/terrain_component.h"

#include "game_world.h"

#include <scieppend/core/ecs_world.h>

void add_terrain_component(EntityHandle e_h, int terrain_id)
{
    struct ECSWorld* world = game_world_get();
    ecs_world_entity_add_component(world, e_h, COMPONENT_TYPE_ID(TerrainComponent));
    struct TerrainComponent* ter_c = ecs_world_entity_get_component(world, e_h, COMPONENT_TYPE_ID(TerrainComponent), WRITE);
    ter_c->id = terrain_id;
    ecs_world_entity_unget_component(world, e_h, COMPONENT_TYPE_ID(TerrainComponent), WRITE);
}

int terrain_component_get_id(EntityHandle e_h)
{
    int id = -1;
    struct TerrainComponent* ter_c = ecs_world_entity_get_component(game_world_get(), e_h, COMPONENT_TYPE_ID(TerrainComponent), READ);
    if(ter_c != NULL)
    {
        id = ter_c->id;
        ecs_world_entity_unget_component(game_world_get(), e_h, COMPONENT_TYPE_ID(TerrainComponent), READ);
    }

    return id;
}

void terrain_component_set_id(EntityHandle e_h, int id)
{
    struct TerrainComponent* ter_c = ecs_world_entity_get_component(game_world_get(), e_h, COMPONENT_TYPE_ID(TerrainComponent), WRITE);
    if(ter_c != NULL)
    {
        ter_c->id = id;
        ecs_world_entity_unget_component(game_world_get(), e_h, COMPONENT_TYPE_ID(TerrainComponent), WRITE);
    }
}

