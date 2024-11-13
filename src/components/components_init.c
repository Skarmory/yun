#include "components/components_init.h"

#include <scieppend/core/ecs.h>
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

DECL_COMPONENT_NAME_ID(AIComponent);
DECL_COMPONENT_NAME_ID(CameraComponent);
DECL_COMPONENT_NAME_ID(FeatureComponent);
DECL_COMPONENT_NAME_ID(GFXComponent);
DECL_COMPONENT_NAME_ID(GFXTerminalComponent);
DECL_COMPONENT_NAME_ID(PlayerComponent);
DECL_COMPONENT_NAME_ID(PositionComponent);
DECL_COMPONENT_NAME_ID(MapComponent);
DECL_COMPONENT_NAME_ID(MovementComponent);
DECL_COMPONENT_NAME_ID(TerrainComponent);
DECL_COMPONENT_NAME_ID(TurnComponent);
DECL_COMPONENT_NAME_ID(TurnControllerComponent);
DECL_COMPONENT_NAME_ID(TurnTakerComponent);

void components_init(void)
{
    INIT_COMPONENT(AIComponent);
    INIT_COMPONENT(CameraComponent);
    INIT_COMPONENT(FeatureComponent);
    INIT_COMPONENT(GFXComponent);
    INIT_COMPONENT(GFXTerminalComponent);
    INIT_COMPONENT(PlayerComponent);
    INIT_COMPONENT(PositionComponent);
    INIT_COMPONENT(MapComponent);
    INIT_COMPONENT(MovementComponent);
    INIT_COMPONENT(TerrainComponent);
    INIT_COMPONENT(TurnComponent);
    INIT_COMPONENT(TurnControllerComponent);
    INIT_COMPONENT(TurnTakerComponent);
}

void components_uninit(void)
{
    UNINIT_COMPONENT(AIComponent);
    UNINIT_COMPONENT(CameraComponent);
    UNINIT_COMPONENT(FeatureComponent);
    UNINIT_COMPONENT(GFXComponent);
    UNINIT_COMPONENT(GFXTerminalComponent);
    UNINIT_COMPONENT(PlayerComponent);
    UNINIT_COMPONENT(PositionComponent);
    UNINIT_COMPONENT(MapComponent);
    UNINIT_COMPONENT(MovementComponent);
    UNINIT_COMPONENT(TerrainComponent);
    UNINIT_COMPONENT(TurnComponent);
    UNINIT_COMPONENT(TurnControllerComponent);
    UNINIT_COMPONENT(TurnTakerComponent);
}
