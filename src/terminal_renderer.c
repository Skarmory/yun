#include "terminal_renderer.h"

#include "components/camera_component.h"
#include "components/gfx_terminal_component.h"
#include "components/position_component.h"
#include "game_world.h"

#include <scieppend/core/array_threadsafe.h>
#include <scieppend/core/comparator.h>
#include <scieppend/core/ecs_events.h>
#include <scieppend/core/ecs_world.h>
#include <scieppend/core/event.h>
#include <scieppend/core/geom.h>
#include <scieppend/core/log.h>
#include <scieppend/core/math_utils.h>
#include <scieppend/core/term.h>
#include <stddef.h>

#define CACHED_COMPONENTS_START_CAPACITY 1024

struct _CachedGFX
{
    ComponentHandle position;
    ComponentHandle gfx;
};

struct _CachedCamera
{
    ComponentHandle position;
    ComponentHandle camera;
};

static struct TerminalRenderer
{
    struct
    {
        struct Array_ThreadSafe entity_handles[3];
        struct Array_ThreadSafe components[3];
        struct _CachedCamera camera;
    } cached_data;

    ObserverHandle observer;
    int frame;
} _tr;

static void _tr_observer_callback([[maybe_unused]]struct Event* event, [[maybe_unused]]void* obs_data, void* event_args)
{
    struct ComponentEventArgs* args = event_args;

    switch(component_event_get_event_type(args))
    {
        case EVENT_COMPONENT_ADDED:
            ComponentHandle pos_h = ecs_world_entity_get_component_handle(game_world_get(), args->base.entity_handle, COMPONENT_TYPE_ID(PositionComponent));
            ComponentHandle gfx_h = ecs_world_entity_get_component_handle(game_world_get(), args->base.entity_handle, COMPONENT_TYPE_ID(GFXTerminalComponent));

            if(pos_h != C_NULL_COMPONENT_HANDLE && gfx_h != C_NULL_COMPONENT_HANDLE)
            {
                struct _CachedGFX cc;
                cc.position = pos_h;
                cc.gfx = gfx_h;

                const struct PositionComponent* pos_c = ecs_world_get_component(game_world_get(), pos_h, COMPONENT_TYPE_ID(PositionComponent), READ);

                struct Array_ThreadSafe* entities_by_layer = &_tr.cached_data.entity_handles[pos_c->z];
                struct Array_ThreadSafe* components_by_layer = &_tr.cached_data.components[pos_c->z];

                ecs_world_unget_component(game_world_get(), pos_h, COMPONENT_TYPE_ID(PositionComponent), READ);

                array_ts_add(entities_by_layer, &args->base.entity_handle);
                array_ts_add(components_by_layer, &cc);
            }
            break;

        case EVENT_COMPONENT_REMOVED:
            for(int layer = 0; layer < 3; ++layer)
            {
                int idx = array_ts_find(&_tr.cached_data.entity_handles[layer], &args->base.entity_handle, &compare_int);
                if(idx != -1)
                {
                    array_ts_remove_at(&_tr.cached_data.entity_handles[layer], idx);
                    array_ts_remove_at(&_tr.cached_data.components[layer], idx);
                    break;
                }
            }
            break;

        default:
            break;
    }
}

// EXTERNAL FUNCS

void terminal_renderer_init(void)
{
    for(int layer = 0; layer < 3; ++layer)
    {
        array_ts_init(&_tr.cached_data.entity_handles[layer], sizeof(EntityHandle), CACHED_COMPONENTS_START_CAPACITY, NULL,NULL);
        array_ts_init(&_tr.cached_data.components[layer], sizeof(struct _CachedGFX), CACHED_COMPONENTS_START_CAPACITY, NULL,NULL);
    }

    _tr.frame = 0;
    _tr.observer = observer_create(NULL, &_tr_observer_callback);
    ecs_world_component_type_register_observer(game_world_get(), COMPONENT_TYPE_ID(PositionComponent), _tr.observer);
    ecs_world_component_type_register_observer(game_world_get(), COMPONENT_TYPE_ID(GFXTerminalComponent), _tr.observer);
}

void terminal_renderer_uninit(void)
{
    ecs_world_component_type_deregister_observer(game_world_get(), COMPONENT_TYPE_ID(GFXTerminalComponent), _tr.observer);
    ecs_world_component_type_deregister_observer(game_world_get(), COMPONENT_TYPE_ID(PositionComponent), _tr.observer);
    observer_destroy(_tr.observer);

    for(int layer = 0; layer < 3; ++layer)
    {
        array_ts_uninit(&_tr.cached_data.components[layer]);
        array_ts_uninit(&_tr.cached_data.entity_handles[layer]);
    }
}

void terminal_renderer_render(void)
{
    log_format_msg(LOG_DEBUG, "Begin frame %d", _tr.frame++);

    const struct CameraComponent* cam = ecs_world_get_component(game_world_get(), _tr.cached_data.camera.camera, COMPONENT_TYPE_ID(CameraComponent), READ);
    const struct PositionComponent* cpos = ecs_world_get_component(game_world_get(), _tr.cached_data.camera.position, COMPONENT_TYPE_ID(PositionComponent), READ);

    int cam_w = cam->width;
    int cam_h = cam->height;

    int Cox = cpos->x - (cam_w / 2);
    int Coy = cpos->y - (cam_h / 2);

    ecs_world_unget_component(game_world_get(), _tr.cached_data.camera.position, COMPONENT_TYPE_ID(PositionComponent), READ);
    ecs_world_unget_component(game_world_get(), _tr.cached_data.camera.camera, COMPONENT_TYPE_ID(CameraComponent), READ);

    //int xstart = clamp(cpos->x - (_tr.dimensions.w / 2), 0, (g_cmap->width * g_map_cell_width) - MCOLS - 1);
    //int ystart = clamp(cpos->y - (_tr.dimensions.h / 2), 0, (g_cmap->height * g_map_cell_height) - MROWS - 1);

    int Sx = -1;
    int Sy = -1;
    int Sw = -1;
    int Sh = -1;

    term_get_wh(&Sw, &Sh);

    Sw = minu(Sw, cam_w);
    Sh = minu(Sh, cam_h);

    for(int layer = 0; layer < 3; ++layer)
    {
        struct Array_ThreadSafe* entities = &_tr.cached_data.entity_handles[layer];
        struct Array_ThreadSafe* components = &_tr.cached_data.components[layer];

        rwlock_read_lock(&entities->lock);
        rwlock_read_lock(&components->lock);
        {
            int component_count = array_count(&components->array);
            for(int i = 0; i < component_count; ++i)
            {
                struct _CachedGFX* cc = array_get(&components->array, i);

                const struct PositionComponent* pc = ecs_world_get_component(game_world_get(), cc->position, COMPONENT_TYPE_ID(PositionComponent), READ);

                // Convert renderable's position to camera coordinates
                math_change_basis(pc->x, pc->y, Cox, Coy, 0, 0, &Sx, &Sy);

                if(geom_point_in_rect(Sx, Sy, 0, 0, Sw, Sh))
                {
                    // GFXComponent in render bounds
                    const struct GFXTerminalComponent* gfxc = ecs_world_get_component(game_world_get(), cc->gfx, COMPONENT_TYPE_ID(GFXTerminalComponent), READ);
                    term_draw_symbol(Sx, Sy, &gfxc->symbol.fg, &gfxc->symbol.bg, gfxc->symbol.attr, gfxc->symbol.sym);
                    ecs_world_unget_component(game_world_get(), cc->gfx, COMPONENT_TYPE_ID(GFXTerminalComponent), READ);
                }

                ecs_world_unget_component(game_world_get(), cc->position, COMPONENT_TYPE_ID(PositionComponent), READ);
            }
        }
        rwlock_read_unlock(&entities->lock);
        rwlock_read_unlock(&components->lock);
    }
}

void terminal_renderer_set_active_camera(ComponentHandle camera, ComponentHandle camera_position)
{
    struct CameraComponent* camera_c = NULL;
    if(_tr.cached_data.camera.camera == C_NULL_COMPONENT_HANDLE)
    {
        camera_c = ecs_world_get_component(game_world_get(), _tr.cached_data.camera.camera, COMPONENT_TYPE_ID(CameraComponent), WRITE);
        camera_c->active = false;
        ecs_world_unget_component(game_world_get(), _tr.cached_data.camera.camera, COMPONENT_TYPE_ID(CameraComponent), WRITE);
    }

    camera_c = ecs_world_get_component(game_world_get(), camera, COMPONENT_TYPE_ID(CameraComponent), WRITE);
    camera_c->active = true;
    ecs_world_unget_component(game_world_get(), camera, COMPONENT_TYPE_ID(CameraComponent), WRITE);

    _tr.cached_data.camera.camera = camera;
    _tr.cached_data.camera.position = camera_position;
}
