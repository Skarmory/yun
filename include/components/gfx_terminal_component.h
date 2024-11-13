#ifndef YUN_COMPONENTS_GFX_TERMINAL_COMPONENT_H
#define YUN_COMPONENTS_GFX_TERMINAL_COMPONENT_H

#include "components/component_decl.h"

#include <scieppend/core/symbol.h>

DECL_COMPONENT(GFXTerminalComponent)
{
    const struct Symbol* base_symbol;
    struct Symbol        symbol;
};

#endif
