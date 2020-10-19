#include "feature.h"

#include <stddef.h>
#include <string.h>

struct Feature* g_features = NULL;
int g_features_count = 0;

struct Feature* feature_look_up_by_id(const char* id)
{
    for(int idx = 0; idx < g_features_count; ++idx)
    {
        if(strcmp(g_features[idx].id, id) == 0)
        {
            return &g_features[idx];
        }
    }

    return NULL;
}
