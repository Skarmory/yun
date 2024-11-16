#include "entities.h"

#include "components/feature_component.h"
#include "components/gfx_terminal_component.h"
#include "components/position_component.h"
#include "components/terrain_component.h"

void add_position_component(EntityHandle e_h, const int x, const int y, const int z)
{
    ComponentHandle pos_h = entity_add_component(e_h, g_PositionComponent_id);
    struct PositionComponent* pos_c = component_get(pos_h, g_PositionComponent_id);
    pos_c->x = x;
    pos_c->y = y;
    pos_c->z = z;
    component_unget(pos_h, g_PositionComponent_id);
}

void add_gfx_component(EntityHandle e_h, const struct Symbol* symbol)
{
    ComponentHandle gfx_h = entity_add_component(e_h, g_GFXTerminalComponent_id);
    struct GFXTerminalComponent* gfx_c = component_get(gfx_h, g_GFXTerminalComponent_id);
    gfx_c->base_symbol = symbol;
    gfx_c->symbol = *symbol;
    component_unget(gfx_h, g_GFXTerminalComponent_id);
}

void add_terrain_component(EntityHandle e_h, int terrain_id)
{
    ComponentHandle ter_h = entity_add_component(e_h, g_TerrainComponent_id);
    struct TerrainComponent* ter_c = component_get(ter_h, g_TerrainComponent_id);
    ter_c->id = terrain_id;
    component_unget(ter_h, g_TerrainComponent_id);
}

void add_feature_component(EntityHandle e_h, int feature_id)
{
    ComponentHandle fet_h = entity_add_component(e_h, g_FeatureComponent_id);
    struct FeatureComponent* fet_c = component_get(fet_h, g_FeatureComponent_id);
    fet_c->id = feature_id;
    component_unget(fet_h, g_FeatureComponent_id);
}
