#include "terrain.h"

#include <scieppend/core/list.h>
#include <scieppend/core/log.h>
#include <scieppend/core/parser.h>

#include <stdlib.h>
#include <string.h>

static struct Terrain* s_terrains = NULL;
static int s_terrains_count = 0;

const struct Terrain* terrain_look_up_by_id(const char* id)
{
    for(int i = 0; i < s_terrains_count; ++i)
    {
        if(strcmp(s_terrains[i].id, id) == 0)
        {
            return &s_terrains[i];
        }
    }

    return NULL;
}

void terrain_parsing_finalise(struct Parser* parser)
{
    struct List* terrains_data = parser_get_userdata(parser);
    struct ListNode* node = NULL;

    log_format_msg(LOG_DEBUG, "Parsed terrains count: %d", terrains_data->count);

    s_terrains = malloc(sizeof(struct Terrain) * terrains_data->count);

    list_for_each(terrains_data, node)
    {
        memcpy(&s_terrains[s_terrains_count++], node->data, sizeof(struct Terrain));
        free(node->data);
    }
}
