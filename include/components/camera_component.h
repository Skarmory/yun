#ifndef YUN_COMPONENTS_CAMERA_COMPONENT_H
#define YUN_COMPONENTS_CAMERA_COMPONENT_H

#include <scieppend/core/component.h>

COMPONENT_TYPE_DECL(CameraComponent)
{
    int  width;
    int  height;
    bool active;
};

void add_camera_component(EntityHandle e_h, int width, int height);

#endif
