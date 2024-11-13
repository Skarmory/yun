#include "terminal_renderer.h"

#include "components/camera_component.h"
#include "components/gfx_terminal_component.h"
#include "components/position_component.h"

#include <scieppend/core/array_threadsafe.h>
#include <scieppend/core/comparator.h>
#include <scieppend/core/ecs.h>
#include <scieppend/core/event.h>
#include <scieppend/core/geom.h>
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
} _tr;

static void _tr_observer_callback([[maybe_unused]]struct Event* event, [[maybe_unused]]void* obs_data, void* event_args)
{
    struct ComponentEventArgs* args = event_args;

    switch(args->event_type)
    {
        case EVENT_COMPONENT_ADDED:
            ComponentHandle pos_h = entity_get_component_handle(args->entity_handle, g_PositionComponent_id);
            ComponentHandle gfx_h = entity_get_component_handle(args->entity_handle, g_GFXTerminalComponent_id);
            //struct CameraComponent* cam_c = entity_get_component(args->entity_handle, g_CameraComponent_id);

            if(pos_h != C_NULL_COMPONENT_HANDLE && gfx_h != C_NULL_COMPONENT_HANDLE)
            {
                struct _CachedGFX cc;
                cc.position = pos_h;
                cc.gfx = gfx_h;

                const struct PositionComponent* pos_c = component_get_readonly(pos_h, g_PositionComponent_id);

                struct Array_ThreadSafe* entities_by_layer = &_tr.cached_data.entity_handles[pos_c->z];
                struct Array_ThreadSafe* components_by_layer = &_tr.cached_data.components[pos_c->z];

                component_unget_readonly(pos_h, g_PositionComponent_id);

                array_ts_add(entities_by_layer, &args->entity_handle);                
                array_ts_add(components_by_layer, &cc);                
            }
            break;

        case EVENT_COMPONENT_REMOVED:
            for(int layer = 0; layer < 3; ++layer)
            {
                int idx = array_ts_find(&_tr.cached_data.entity_handles[layer], &args->entity_handle, &compare_int);
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

void terminal_renderer_init(void)
{
    for(int layer = 0; layer < 3; ++layer)
    {
        array_ts_init(&_tr.cached_data.entity_handles[layer], sizeof(EntityHandle), CACHED_COMPONENTS_START_CAPACITY, NULL,NULL);
        array_ts_init(&_tr.cached_data.components[layer], sizeof(struct _CachedGFX), CACHED_COMPONENTS_START_CAPACITY, NULL,NULL);
    }

    _tr.observer = observer_create(NULL, &_tr_observer_callback);
    component_type_added_register_observer(g_PositionComponent_id, _tr.observer);
    component_type_added_register_observer(g_GFXTerminalComponent_id, _tr.observer);
    //component_type_added_register_observer(g_CameraComponent_id, _tr.observer);
    component_type_removed_register_observer(g_PositionComponent_id, _tr.observer);
    component_type_removed_register_observer(g_GFXTerminalComponent_id, _tr.observer);
    //component_type_removed_register_observer(g_CameraComponent_id, _tr.observer);
}

void terminal_renderer_uninit(void)
{
    //component_type_removed_deregister_observer(g_CameraComponent_id, _tr.observer);
    component_type_removed_deregister_observer(g_GFXTerminalComponent_id, _tr.observer);
    component_type_removed_deregister_observer(g_PositionComponent_id, _tr.observer);
    //component_type_added_deregister_observer(g_CameraComponent_id, _tr.observer);
    component_type_added_deregister_observer(g_GFXTerminalComponent_id, _tr.observer);
    component_type_added_deregister_observer(g_PositionComponent_id, _tr.observer);
    observer_destroy(_tr.observer);

    for(int layer = 0; layer < 3; ++layer)
    {
        array_ts_uninit(&_tr.cached_data.components[layer]);
        array_ts_uninit(&_tr.cached_data.entity_handles[layer]);
    }
}

void terminal_renderer_render(void)
{
    const struct CameraComponent* cam = component_get_readonly(_tr.cached_data.camera.camera, g_CameraComponent_id);
    const struct PositionComponent* cpos = component_get_readonly(_tr.cached_data.camera.position, g_PositionComponent_id);

    int Cox = cpos->x - (cam->width / 2);
    int Coy = cpos->y - (cam->height / 2);

    component_unget_readonly(_tr.cached_data.camera.position, g_PositionComponent_id);

    //int xstart = clamp(cpos->x - (_tr.dimensions.w / 2), 0, (g_cmap->width * g_map_cell_width) - MCOLS - 1);
    //int ystart = clamp(cpos->y - (_tr.dimensions.h / 2), 0, (g_cmap->height * g_map_cell_height) - MROWS - 1);

    int Sx = -1;
    int Sy = -1;
    int Sw = -1;
    int Sh = -1;

    term_get_wh(&Sw, &Sh);

    Sw = minu(Sw, cam->width);
    Sh = minu(Sh, cam->height);

    component_unget_readonly(_tr.cached_data.camera.camera, g_CameraComponent_id);

    for(int layer = 0; layer < 3; ++layer)
    {
        struct Array_ThreadSafe* entities = &_tr.cached_data.entity_handles[layer];
        struct Array_ThreadSafe* components = &_tr.cached_data.components[layer];

        rwlock_read_lock(&entities->lock);
        rwlock_read_lock(&components->lock);
        {
            for(int i = 0; i < array_count(&components->array); ++i)
            {
                struct _CachedGFX* cc = array_get(&components->array, i);

                const struct PositionComponent* pc = component_get_readonly(cc->position, g_PositionComponent_id);

                // Convert renderable's position to camera coordinates
                math_change_basis(pc->x, pc->y, Cox, Coy, 0, 0, &Sx, &Sy);

                if(geom_point_in_rect(Sx, Sy, 0, 0, Sw, Sh))
                {
                    // GFXComponent in render bounds
                    const struct GFXTerminalComponent* gfxc = component_get_readonly(cc->gfx, g_GFXTerminalComponent_id);
                    term_draw_symbol(Sx, Sy, &gfxc->symbol.fg, &gfxc->symbol.bg, gfxc->symbol.attr, gfxc->symbol.sym);
                    component_unget_readonly(cc->gfx, g_GFXTerminalComponent_id);
                }

                component_unget_readonly(cc->position, g_PositionComponent_id);
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
        camera_c = component_get(_tr.cached_data.camera.camera, g_CameraComponent_id);
        camera_c->active = false;
        component_unget(_tr.cached_data.camera.camera, g_CameraComponent_id);
    }

    camera_c = component_get(camera, g_CameraComponent_id);
    camera_c->active = true;
    component_unget(camera, g_CameraComponent_id);

    _tr.cached_data.camera.camera = camera;
    _tr.cached_data.camera.position = camera_position;
}
