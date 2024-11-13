#ifndef YUN_COMPONENTS_CAMERA_COMPONENT_H
#define YUN_COMPONENTS_CAMERA_COMPONENT_H

#include "components/component_decl.h"

DECL_COMPONENT(CameraComponent)
{
    int  width;
    int  height;
    bool active;
};

#endif
