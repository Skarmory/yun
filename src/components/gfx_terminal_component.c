#include "components/gfx_terminal_component.h"

#include "game_world.h"

#include <scieppend/core/ecs_world.h>
#include <scieppend/core/symbol.h>

void add_gfx_terminal_component(EntityHandle e_h, const struct Symbol* symbol)
{
    struct ECSWorld* world = game_world_get();
    ecs_world_entity_add_component(world, e_h, COMPONENT_TYPE_ID(GFXTerminalComponent));
    struct GFXTerminalComponent* gfx_c = ecs_world_entity_get_component(world, e_h, COMPONENT_TYPE_ID(GFXTerminalComponent), WRITE);
    gfx_c->base_symbol = symbol;
    gfx_c->symbol = *symbol;
    ecs_world_entity_unget_component(world, e_h, COMPONENT_TYPE_ID(GFXTerminalComponent), WRITE);
}

void gfx_terminal_component_set(EntityHandle e_h, const struct Symbol* base_symbol, const struct Symbol* symbol)
{
    struct GFXTerminalComponent* gfx_c = ecs_world_entity_get_component(game_world_get(), e_h, COMPONENT_TYPE_ID(GFXTerminalComponent), WRITE);
    if(gfx_c != NULL)
    {
        gfx_c->base_symbol = base_symbol;
        gfx_c->symbol = *symbol;
        ecs_world_entity_unget_component(game_world_get(), e_h, COMPONENT_TYPE_ID(GFXTerminalComponent), WRITE);
    }
}

void gfx_terminal_component_set_base_symbol(EntityHandle e_h, const struct Symbol* symbol)
{
    struct GFXTerminalComponent* gfx_c = ecs_world_entity_get_component(game_world_get(), e_h, COMPONENT_TYPE_ID(GFXTerminalComponent), WRITE);
    if(gfx_c != NULL)
    {
        gfx_c->base_symbol = symbol;
        ecs_world_entity_unget_component(game_world_get(), e_h, COMPONENT_TYPE_ID(GFXTerminalComponent), WRITE);
    }
}

void gfx_terminal_component_set_symbol(EntityHandle e_h, const struct Symbol* symbol)
{
    struct GFXTerminalComponent* gfx_c = ecs_world_entity_get_component(game_world_get(), e_h, COMPONENT_TYPE_ID(GFXTerminalComponent), WRITE);
    if(gfx_c != NULL)
    {
        gfx_c->symbol = *symbol;
        ecs_world_entity_unget_component(game_world_get(), e_h, COMPONENT_TYPE_ID(GFXTerminalComponent), WRITE);
    }
}
