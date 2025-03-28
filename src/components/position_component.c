#include "components/position_component.h"

#include "game_world.h"

#include <scieppend/core/ecs_world.h>

void add_position_component(EntityHandle e_h, const int x, const int y, const int z)
{
    struct ECSWorld* world = game_world_get();
    ecs_world_entity_add_component(world, e_h, COMPONENT_TYPE_ID(PositionComponent));
    struct PositionComponent* pos_c = ecs_world_entity_get_component(world, e_h, COMPONENT_TYPE_ID(PositionComponent), WRITE);
    pos_c->x = x;
    pos_c->y = y;
    pos_c->z = z;
    ecs_world_entity_unget_component(world, e_h, COMPONENT_TYPE_ID(PositionComponent), WRITE);
}

void position_component_set_xyz(EntityHandle e_h, const int x, const int y, const int z)
{
    struct ECSWorld* world = game_world_get();
    struct PositionComponent* pos_c = ecs_world_entity_get_component(world, e_h, COMPONENT_TYPE_ID(PositionComponent), WRITE);
    pos_c->x = x;
    pos_c->y = y;
    pos_c->z = z;
    ecs_world_entity_unget_component(world, e_h, COMPONENT_TYPE_ID(PositionComponent), WRITE);
}
