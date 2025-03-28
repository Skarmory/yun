#include "components/components_init.h"

#include <scieppend/core/component.h>
#include <scieppend/core/ecs_world.h>
#include <scieppend/core/string.h>

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

COMPONENT_TYPE_DEF(AIComponent);
COMPONENT_TYPE_DEF(CameraComponent);
COMPONENT_TYPE_DEF(FeatureComponent);
COMPONENT_TYPE_DEF(GFXComponent);
COMPONENT_TYPE_DEF(GFXTerminalComponent);
COMPONENT_TYPE_DEF(PlayerComponent);
COMPONENT_TYPE_DEF(PositionComponent);
COMPONENT_TYPE_DEF(MapComponent);
COMPONENT_TYPE_DEF(MovementComponent);
COMPONENT_TYPE_DEF(TerrainComponent);
COMPONENT_TYPE_DEF(TurnComponent);
COMPONENT_TYPE_DEF(TurnControllerComponent);
COMPONENT_TYPE_DEF(TurnTakerComponent);

void components_init(void)
{
    COMPONENT_TYPE_INIT(AIComponent);
    COMPONENT_TYPE_INIT(CameraComponent);
    COMPONENT_TYPE_INIT(FeatureComponent);
    COMPONENT_TYPE_INIT(GFXComponent);
    COMPONENT_TYPE_INIT(GFXTerminalComponent);
    COMPONENT_TYPE_INIT(PlayerComponent);
    COMPONENT_TYPE_INIT(PositionComponent);
    COMPONENT_TYPE_INIT(MapComponent);
    COMPONENT_TYPE_INIT(MovementComponent);
    COMPONENT_TYPE_INIT(TerrainComponent);
    COMPONENT_TYPE_INIT(TurnComponent);
    COMPONENT_TYPE_INIT(TurnControllerComponent);
    COMPONENT_TYPE_INIT(TurnTakerComponent);
}

void components_uninit(void)
{
    COMPONENT_TYPE_UNINIT(AIComponent);
    COMPONENT_TYPE_UNINIT(CameraComponent);
    COMPONENT_TYPE_UNINIT(FeatureComponent);
    COMPONENT_TYPE_UNINIT(GFXComponent);
    COMPONENT_TYPE_UNINIT(GFXTerminalComponent);
    COMPONENT_TYPE_UNINIT(PlayerComponent);
    COMPONENT_TYPE_UNINIT(PositionComponent);
    COMPONENT_TYPE_UNINIT(MapComponent);
    COMPONENT_TYPE_UNINIT(MovementComponent);
    COMPONENT_TYPE_UNINIT(TerrainComponent);
    COMPONENT_TYPE_UNINIT(TurnComponent);
    COMPONENT_TYPE_UNINIT(TurnControllerComponent);
    COMPONENT_TYPE_UNINIT(TurnTakerComponent);
}
