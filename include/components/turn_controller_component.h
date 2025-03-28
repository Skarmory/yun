#ifndef YUN_COMPONENTS_TURN_CONTROLLER_COMPONENT_H
#define YUN_COMPONENTS_TURN_CONTROLLER_COMPONENT_H

#include <scieppend/core/array_threadsafe.h>
#include <scieppend/core/component.h>
#include <scieppend/core/event.h>

// TODO, does this need to be a component?

COMPONENT_TYPE_DECL(TurnControllerComponent)
{
    int turn_number;
    int initiative_step;
    int turn_takers_awaiting;
    struct Array_ThreadSafe turn_takers;
    ObserverHandle observer;
};

#endif
