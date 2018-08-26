#ifndef PATH_H
#define PATH_H

#include "map.h"

struct PathNode
{
    struct Location* loc;
    struct PathNode* pathlist_next;
    struct PathNode* next;
    struct PathNode* prev;
    float cost_to_end;
    float cost_from_start;
};

struct Location* next_path_loc(struct Location* start, struct Location* dest, int path_bits);

inline struct PathNode* get_first_path_node(struct PathNode* node)
{
    if(!node->prev)
        return node;

    while(node->prev->prev)
        node = node->prev;

    return node;
}

#endif
