#ifndef YUN_COMPONENTS_FEATURE_COMPONENT_H
#define YUN_COMPONENTS_FEATURE_COMPONENT_H

#include <scieppend/core/component.h>

COMPONENT_TYPE_DECL(FeatureComponent)
{
    int id;
};

void add_feature_component(EntityHandle e_h, int feature_id);

int feature_component_get_id(EntityHandle e_h);
void feature_component_set_id(EntityHandle e_h, int new_id);

#endif

