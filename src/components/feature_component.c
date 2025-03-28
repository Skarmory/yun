#include "components/feature_component.h"

#include "game_world.h"

#include <scieppend/core/ecs_world.h>

void add_feature_component(EntityHandle e_h, int feature_id)
{
    struct ECSWorld* world = game_world_get();
    ecs_world_entity_add_component(world, e_h, COMPONENT_TYPE_ID(FeatureComponent));
    struct FeatureComponent* fet_c = ecs_world_entity_get_component(world, e_h, COMPONENT_TYPE_ID(FeatureComponent), WRITE);
    fet_c->id = feature_id;
    ecs_world_entity_unget_component(world, e_h, COMPONENT_TYPE_ID(FeatureComponent), WRITE);
}

int feature_component_get_id(EntityHandle e_h)
{
    int id = -1;
    struct FeatureComponent* fc = ecs_world_entity_get_component(game_world_get(), e_h, COMPONENT_TYPE_ID(FeatureComponent), READ);
    if(fc != NULL)
    {
        id = fc->id;
        ecs_world_entity_unget_component(game_world_get(), e_h, COMPONENT_TYPE_ID(FeatureComponent), READ);
    }
    return id;
}

void feature_component_set_id(EntityHandle e_h, int new_id)
{
    struct FeatureComponent* fc = ecs_world_entity_get_component(game_world_get(), e_h, COMPONENT_TYPE_ID(FeatureComponent), WRITE);
    if(fc != NULL)
    {
        fc->id = new_id;
        ecs_world_entity_unget_component(game_world_get(), e_h, COMPONENT_TYPE_ID(FeatureComponent), WRITE);
    }
}
