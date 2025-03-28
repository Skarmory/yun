#include "feature.h"

#include <scieppend/core/hash.h>
#include <stddef.h>
#include <string.h>

struct Feature* g_features = NULL;
int g_features_count = 0;

const struct Feature* feature_look_up_by_id(const char* id)
{
    int id_hash = hash(id, strlen(id));
    return feature_look_up_by_id_hash(id_hash);
}

const struct Feature* feature_look_up_by_id_hash(int id_hash)
{
    for(int idx = 0; idx < g_features_count; ++idx)
    {
        if(g_features[idx].id_hash == id_hash)
        {
            return &g_features[idx];
        }
    }

    return NULL;
}
