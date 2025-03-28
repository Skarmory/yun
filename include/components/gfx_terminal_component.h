#ifndef YUN_COMPONENTS_GFX_TERMINAL_COMPONENT_H
#define YUN_COMPONENTS_GFX_TERMINAL_COMPONENT_H

#include <scieppend/core/component.h>
#include <scieppend/core/symbol.h>

COMPONENT_TYPE_DECL(GFXTerminalComponent)
{
    const struct Symbol* base_symbol;
    struct Symbol        symbol;
};

void add_gfx_terminal_component(EntityHandle e_h, const struct Symbol* symbol);
void gfx_terminal_component_set(EntityHandle e_h, const struct Symbol* base_symbol, const struct Symbol* symbol);
void gfx_terminal_component_set_base_symbol(EntityHandle e_h, const struct Symbol* symbol);
void gfx_terminal_component_set_symbol(EntityHandle e_h, const struct Symbol* symbol);

#endif
