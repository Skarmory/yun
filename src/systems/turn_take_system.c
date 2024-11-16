#include "systems/turn_take_system.h"

#include "components/ai_component.h"
#include "components/player_component.h"
#include "components/turn_component.h"

#include <scieppend/core/ecs.h>
#include <scieppend/core/event.h>
#include <scieppend/core/input_keycodes.h>
#include <scieppend/core/log.h>
#include <scieppend/core/string.h>

#include <stddef.h>

struct TurnTakeSystem
{
    int system_id;
    struct CommandManagerComponent* cmd_mgr;
    ObserverHandle observer;
};

static struct TurnTakeSystem _player_tts;
static struct TurnTakeSystem _ai_tts;

static void _player_tts_update(EntityHandle entity_handle)
{
    //enum GameplayCommand cmd = (enum GameplayCommand)get_key();
    //switch(cmd)
    //{
    //}

    entity_remove_component(entity_handle, g_TurnComponent_id);
}

static void _ai_tts_update(EntityHandle entity_handle)
{
    entity_remove_component(entity_handle, g_TurnComponent_id);
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
    array_add(&required_components, &g_TurnComponent_id);
    array_add(&required_components, &g_PlayerComponent_id);

    _player_tts.system_id = system_register(&sysname, &_player_tts_update, &required_components);

    array_uninit(&required_components);
    array_init(&required_components, sizeof(int), 1, NULL, NULL); 
    string_set(&sysname, "AITurnTakeSystem");
    array_add(&required_components, &g_TurnComponent_id);
    array_add(&required_components, &g_AIComponent_id);

    _ai_tts.system_id = system_register(&sysname, &_ai_tts_update, &required_components);

    array_uninit(&required_components);
    string_uninit(&sysname);

    //_player_tts.observer = observer_create(&_player_tts, &_tts_observer_callback);
    //_ai_tts.observer = observer_create(&_ai_tts, &_tts_observer_callback);

    //component_type_added_register_observer(g_CommandManagerComponent_id, _player_tts.observer);
    //component_type_added_register_observer(g_CommandManagerComponent_id, _ai_tts.observer);
}

void turn_take_system_uninit(void)
{
    observer_destroy(_player_tts.observer);
    observer_destroy(_ai_tts.observer);
}
