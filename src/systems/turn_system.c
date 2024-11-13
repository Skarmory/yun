#include "systems/turn_system.h"

#include "components/turn_component.h"
#include "components/turn_controller_component.h"
#include "components/turn_taker_component.h"

#include <scieppend/core/string.h>
#include <scieppend/core/ecs.h>
#include <scieppend/core/event.h>
#include <scieppend/core/array.h>

static struct TurnSystem
{
    int system_id;
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
    struct TurnControllerComponent* tcc = component_get(*(ComponentHandle*)obs_data, g_TurnControllerComponent_id);

    switch(args->event_type)
    {
        case EVENT_COMPONENT_ADDED:
            if(args->component_type == g_TurnTakerComponent_id)
            {
                array_ts_add(&tcc->turn_takers, &args->entity_handle);
            }
            break;
        case EVENT_COMPONENT_REMOVED:
            if(args->component_type == g_TurnTakerComponent_id)
            {
                array_ts_find_and_remove(&tcc->turn_takers, &args->entity_handle, &_compare_entity_handle);
                //int idx = array_find(&tcc->turn_takers, &args->entity_handle, &_compare_entity_handle);
                //if(idx != -1)
                //{
                //    array_remove_at(&tcc->turn_takers, idx);
                //}
            }
            else if(args->component_type == g_TurnComponent_id)
            {
                --tcc->turn_takers_awaiting;
            }
            break;
        default:
            break;
    }

    component_unget(*(ComponentHandle*)obs_data, g_TurnControllerComponent_id);
}

/* Give a TurnComponent to all TurnTakerComponent holders if the initiative matches the current step.
 * TurnController should only be attached to ONE entity (i.e. this system will only run for one entity).
 */
static void _turn_system_update(EntityHandle entity_handle)
{
    struct TurnControllerComponent* tc = entity_get_component(entity_handle, g_TurnControllerComponent_id);

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

            const struct TurnTakerComponent* ttc = entity_get_readonly_component(*turn_taker_handle, g_TurnTakerComponent_id);
            if(ttc->initiative == tc->initiative_step)
            {
                entity_add_component(*turn_taker_handle, g_TurnComponent_id);
                ++tc->turn_takers_awaiting;
            }
            entity_unget_readonly_component(*turn_taker_handle, g_TurnTakerComponent_id);
        }
    }

    entity_unget_component(entity_handle, g_TurnControllerComponent_id);
}

void turn_system_init(void)
{
    struct string sysname;
    string_init(&sysname, "TurnControllerSystem");

    struct Array required_components;
    array_init(&required_components, sizeof(int), 1, NULL, NULL); 
    array_add(&required_components, &g_TurnControllerComponent_id);

    _turn_system.system_id = system_register(&sysname, &_turn_system_update, &required_components);

    array_uninit(&required_components);
    string_uninit(&sysname);

    _turn_system.turn_controller_handle = entity_create();

    _turn_system.turn_controller_component_handle = entity_add_component(_turn_system.turn_controller_handle, g_TurnControllerComponent_id);
    struct TurnControllerComponent* tcc = component_get(_turn_system.turn_controller_component_handle, g_TurnControllerComponent_id);
    {
        array_ts_init(&tcc->turn_takers, sizeof(EntityHandle), 8, NULL, NULL);
        tcc->turn_number = -1;
        tcc->initiative_step = -1;
        tcc->turn_takers_awaiting = 0;
        tcc->observer = observer_create(&_turn_system.turn_controller_component_handle, &_turn_taker_component_callback);

        component_type_added_register_observer(g_TurnTakerComponent_id, tcc->observer);
        component_type_removed_register_observer(g_TurnTakerComponent_id, tcc->observer);
        component_type_removed_register_observer(g_TurnComponent_id, tcc->observer);
    }
    component_unget(_turn_system.turn_controller_component_handle, g_TurnControllerComponent_id);
}

void turn_system_uninit(void)
{
    struct TurnControllerComponent* tcc = entity_get_component(_turn_system.turn_controller_handle, g_TurnControllerComponent_id);
    component_type_removed_register_observer(g_TurnComponent_id, tcc->observer);
    component_type_removed_deregister_observer(g_TurnTakerComponent_id, tcc->observer);
    component_type_added_deregister_observer(g_TurnTakerComponent_id, tcc->observer);
    observer_destroy(tcc->observer);
    array_ts_uninit(&tcc->turn_takers);
    entity_destroy(_turn_system.turn_controller_handle);
}
