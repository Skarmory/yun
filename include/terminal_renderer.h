#ifndef YUN_TERMINAL_RENDERER_H
#define YUN_TERMINAL_RENDERER_H

#include <scieppend/core/ecs.h>

void terminal_renderer_init(void);
void terminal_renderer_uninit(void);
void terminal_renderer_render(void);
void terminal_renderer_set_active_camera(ComponentHandle camera, ComponentHandle camera_position);

#endif
