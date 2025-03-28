#include "systems/turn_take_system.h"

#include "components/ai_component.h"
#include "components/player_component.h"
#include "components/turn_component.h"
#include "game_world.h"

#include <scieppend/core/ecs_events.h>
#include <scieppend/core/ecs_world.h>
#include <scieppend/core/event.h>
#include <scieppend/core/input_keycodes.h>
#include <scieppend/core/log.h>
#include <scieppend/core/string.h>

#include <stddef.h>

struct TurnTakeSystem
{
    struct CommandManagerComponent* cmd_mgr;
    ObserverHandle observer;
};

static void _player_tts_update(struct ECSWorld* world, EntityHandle entity_handle)
{
    //enum GameplayCommand cmd = (enum GameplayCommand)get_key();
    //switch(cmd)
    //{
    //}

    ecs_world_entity_remove_component(world, entity_handle, COMPONENT_TYPE_ID(TurnComponent));
}

static void _ai_tts_update(struct ECSWorld* world, EntityHandle entity_handle)
{
    ecs_world_entity_remove_component(world, entity_handle, COMPONENT_TYPE_ID(TurnComponent));
}

//static void _tts_observer_callback(struct Event* sender, void* observer_data, void* event_args)
//{
//    struct ComponentEventArgs* args = event_args;
//    struct TurnTakeSystem* tts = observer_data;
//    tts->cmd_mgr_c = entity_get_component(args->entity_handle, g_CommandManagerComponent_id);
//}

void turn_take_system_init(void)
{
    struct string sysname;
    string_init(&sysname, "PlayerTurnTakeSystem");

    struct Array required_components;
    array_init(&required_components, sizeof(int), 1, NULL, NULL); 
    array_add(&required_components, &COMPONENT_TYPE_ID(TurnComponent));
    array_add(&required_components, &COMPONENT_TYPE_ID(PlayerComponent));

    //_player_tts.system_id = system_register(&sysname, &_player_tts_update, &required_components);
    ecs_world_system_register(game_world_get(), &sysname, &required_components, &_player_tts_update);

    array_uninit(&required_components);
    array_init(&required_components, sizeof(int), 1, NULL, NULL); 
    string_set(&sysname, "AITurnTakeSystem");
    array_add(&required_components, &COMPONENT_TYPE_ID(TurnComponent));
    array_add(&required_components, &COMPONENT_TYPE_ID(AIComponent));

    //_ai_tts.system_id = system_register(&sysname, &_ai_tts_update, &required_components);
    ecs_world_system_register(game_world_get(), &sysname, &required_components, &_ai_tts_update);

    array_uninit(&required_components);
    string_uninit(&sysname);

    //_player_tts.observer = observer_create(&_player_tts, &_tts_observer_callback);
    //_ai_tts.observer = observer_create(&_ai_tts, &_tts_observer_callback);

    //component_type_added_register_observer(g_CommandManagerComponent_id, _player_tts.observer);
    //component_type_added_register_observer(g_CommandManagerComponent_id, _ai_tts.observer);
}

void turn_take_system_uninit(void)
{
    //observer_destroy(_player_tts.observer);
    //observer_destroy(_ai_tts.observer);
}
