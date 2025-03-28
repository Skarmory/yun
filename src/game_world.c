#include "game_world.h"

#include "components/ai_component.h"
#include "components/camera_component.h"
#include "components/feature_component.h"
#include "components/gfx_component.h"
#include "components/gfx_terminal_component.h"
#include "components/player_component.h"
#include "components/position_component.h"
#include "components/map_component.h"
#include "components/movement_component.h"
#include "components/terrain_component.h"
#include "components/turn_component.h"
#include "components/turn_controller_component.h"
#include "components/turn_taker_component.h"

#include <scieppend/core/component.h>
#include <scieppend/core/ecs_world.h>

static struct ECSWorld* _game_world = NULL;

struct ECSWorld* game_world_get(void)
{
    return _game_world;
}

void game_world_init(void)
{
    _game_world = ecs_world_new();

    ecs_world_component_type_register(_game_world, COMPONENT_TYPE_ID(AIComponent), sizeof(struct AIComponent));
    ecs_world_component_type_register(_game_world, COMPONENT_TYPE_ID(CameraComponent), sizeof(struct CameraComponent));
    ecs_world_component_type_register(_game_world, COMPONENT_TYPE_ID(FeatureComponent), sizeof(struct FeatureComponent));
    ecs_world_component_type_register(_game_world, COMPONENT_TYPE_ID(GFXComponent), sizeof(struct GFXComponent));
    ecs_world_component_type_register(_game_world, COMPONENT_TYPE_ID(GFXTerminalComponent), sizeof(struct GFXTerminalComponent));
    ecs_world_component_type_register(_game_world, COMPONENT_TYPE_ID(PlayerComponent), sizeof(struct PlayerComponent));
    ecs_world_component_type_register(_game_world, COMPONENT_TYPE_ID(PositionComponent), sizeof(struct PositionComponent));
    ecs_world_component_type_register(_game_world, COMPONENT_TYPE_ID(MapComponent), sizeof(struct MapComponent));
    ecs_world_component_type_register(_game_world, COMPONENT_TYPE_ID(MovementComponent), sizeof(struct MovementComponent));
    ecs_world_component_type_register(_game_world, COMPONENT_TYPE_ID(TerrainComponent), sizeof(struct TerrainComponent));
    ecs_world_component_type_register(_game_world, COMPONENT_TYPE_ID(TurnComponent), sizeof(struct TurnComponent));
    ecs_world_component_type_register(_game_world, COMPONENT_TYPE_ID(TurnControllerComponent), sizeof(struct TurnControllerComponent));
    ecs_world_component_type_register(_game_world, COMPONENT_TYPE_ID(TurnTakerComponent), sizeof(struct TurnTakerComponent));
}

void game_world_uninit(void)
{
    ecs_world_free(_game_world);
}

