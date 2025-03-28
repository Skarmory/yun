#include "components/camera_component.h"

#include "game_world.h"

#include <scieppend/core/ecs_world.h>

void add_camera_component(EntityHandle e_h, int width, int height)
{
    struct ECSWorld* world = game_world_get();
    ecs_world_entity_add_component(world, e_h, COMPONENT_TYPE_ID(CameraComponent));
    struct CameraComponent* cam_c = ecs_world_entity_get_component(world, e_h, COMPONENT_TYPE_ID(CameraComponent), WRITE);
    cam_c->width = width;
    cam_c->height = height;
    ecs_world_entity_unget_component(world, e_h, COMPONENT_TYPE_ID(CameraComponent), WRITE);
}
