#include "systems/turn_system.h"

#include "components/turn_component.h"
#include "components/turn_controller_component.h"
#include "components/turn_taker_component.h"
#include "game_world.h"

#include <scieppend/core/array.h>
#include <scieppend/core/component.h>
#include <scieppend/core/ecs_events.h>
#include <scieppend/core/ecs_world.h>
#include <scieppend/core/event.h>
#include <scieppend/core/string.h>

static struct TurnSystem
{
    EntityHandle turn_controller_handle;
    ComponentHandle turn_controller_component_handle;
} _turn_system;

static int _compare_entity_handle(const void* lhs, const void* rhs)
{
    return *(EntityHandle*)lhs == *(EntityHandle*)rhs;
}

static void _turn_taker_component_callback([[maybe_unused]] struct Event* sender, void* obs_data, void* event_args)
{
    struct ComponentEventArgs* args = event_args;
    struct TurnControllerComponent* tcc = ecs_world_get_component(game_world_get(), *(ComponentHandle*)obs_data, COMPONENT_TYPE_ID(TurnControllerComponent), WRITE);

    switch(component_event_get_event_type(args))
    {
        case EVENT_COMPONENT_ADDED:
            if(args->component_type == COMPONENT_TYPE_ID(TurnTakerComponent))
            {
                array_ts_add(&tcc->turn_takers, &args->base.entity_handle);
            }
            break;
        case EVENT_COMPONENT_REMOVED:
            if(args->component_type == COMPONENT_TYPE_ID(TurnTakerComponent))
            {
                array_ts_find_and_remove(&tcc->turn_takers, &args->base.entity_handle, &_compare_entity_handle);
            }
            else if(args->component_type == COMPONENT_TYPE_ID(TurnComponent))
            {
                --tcc->turn_takers_awaiting;
            }
            break;
        default:
            break;
    }

    ecs_world_unget_component(game_world_get(), *(ComponentHandle*)obs_data, COMPONENT_TYPE_ID(TurnControllerComponent), WRITE);
}

/**
 * Give a TurnComponent to all TurnTakerComponent holders if the initiative matches the current step.
 * TurnController should only be attached to ONE entity (i.e. this system will only run for one entity).
 */
static void _turn_system_update(struct ECSWorld* world, EntityHandle entity_handle)
{
    struct TurnControllerComponent* tc = ecs_world_entity_get_component(world, entity_handle, COMPONENT_TYPE_ID(TurnControllerComponent), WRITE);

    if(tc->turn_takers_awaiting > 0)
    {
        tc->initiative_step = (tc->initiative_step + 1) % 10;

        if(tc->initiative_step == 0)
        {
            ++tc->turn_number;
        }

        for(int i = array_count(&tc->turn_takers.array) - 1; i > -1; --i)
        {
            EntityHandle* turn_taker_handle = array_get(&tc->turn_takers.array, i);

            const struct TurnTakerComponent* ttc = ecs_world_entity_get_component(world, *turn_taker_handle, COMPONENT_TYPE_ID(TurnTakerComponent), READ);
            if(ttc->initiative == tc->initiative_step)
            {
                ecs_world_entity_add_component(world, *turn_taker_handle, COMPONENT_TYPE_ID(TurnComponent));
                ++tc->turn_takers_awaiting;
            }
            ecs_world_entity_unget_component(world, *turn_taker_handle, COMPONENT_TYPE_ID(TurnTakerComponent), READ);
        }
    }

    ecs_world_entity_unget_component(world, entity_handle, COMPONENT_TYPE_ID(TurnControllerComponent), WRITE);
}

void turn_system_init(void)
{
    struct string sysname;
    string_init(&sysname, "TurnControllerSystem");

    struct Array required_components;
    array_init(&required_components, sizeof(int), 1, NULL, NULL); 
    array_add(&required_components, &COMPONENT_TYPE_ID(TurnControllerComponent));

    ecs_world_system_register(game_world_get(), &sysname,  &required_components, &_turn_system_update);

    array_uninit(&required_components);
    string_uninit(&sysname);

    _turn_system.turn_controller_handle = ecs_world_create_entity(game_world_get());

    ecs_world_entity_add_component(game_world_get(), _turn_system.turn_controller_handle, COMPONENT_TYPE_ID(TurnControllerComponent));
    struct TurnControllerComponent* tcc = ecs_world_get_component(game_world_get(), _turn_system.turn_controller_component_handle, COMPONENT_TYPE_ID(TurnControllerComponent), WRITE);
    {
        array_ts_init(&tcc->turn_takers, sizeof(EntityHandle), 8, NULL, NULL);
        tcc->turn_number = -1;
        tcc->initiative_step = -1;
        tcc->turn_takers_awaiting = 0;
        tcc->observer = observer_create(&_turn_system.turn_controller_component_handle, &_turn_taker_component_callback);

        ecs_world_component_type_register_observer(game_world_get(), COMPONENT_TYPE_ID(TurnTakerComponent), tcc->observer);
        ecs_world_component_type_register_observer(game_world_get(), COMPONENT_TYPE_ID(TurnComponent), tcc->observer);
    }
    ecs_world_unget_component(game_world_get(), _turn_system.turn_controller_component_handle, COMPONENT_TYPE_ID(TurnControllerComponent), WRITE);
}

void turn_system_uninit(void)
{
    struct TurnControllerComponent* tcc = ecs_world_entity_get_component(game_world_get(), _turn_system.turn_controller_handle, COMPONENT_TYPE_ID(TurnControllerComponent), READ);
    ecs_world_component_type_deregister_observer(game_world_get(), COMPONENT_TYPE_ID(TurnComponent), tcc->observer);
    ecs_world_component_type_deregister_observer(game_world_get(), COMPONENT_TYPE_ID(TurnTakerComponent), tcc->observer);

    observer_destroy(tcc->observer);
    array_ts_uninit(&tcc->turn_takers);
    ecs_world_entity_unget_component(game_world_get(), _turn_system.turn_controller_handle, COMPONENT_TYPE_ID(TurnControllerComponent), READ);
    ecs_world_destroy_entity(game_world_get(), _turn_system.turn_controller_handle);
}
