#include "pathing.h"

#include "globals.h"
#include "log.h"
#include "map.h"
#include "util.h"

#include <float.h>
#include <math.h>
#include <ncurses.h>
#include <stdlib.h>

struct PathNode* _open_head;

/**
 * Walk back through the linked list and get the first node.
 */
struct PathNode* _get_first_path_node(struct PathNode* node)
{
    if(!node->prev)
        return node;

    while(node->prev->prev)
        node = node->prev;

    return node;
}

/**
 * Evaluates a node's cost
 */
float _evaluate(struct PathNode* path, struct Location* dest, int path_bits)
{
    float mod = 0.0f;

    // Special handling for the destination
    if(path->loc->x == dest->x && path->loc->y == dest->y)
        return -INFINITY;

    // If this is not a valid move (e.g. other mon on this loc), assign a higher value than it's distance
    // This means the algorithm will explore other paths before checking further in this direction
    if(!map_valid_move(cmap, path->loc->x, path->loc->y, path_bits))
        mod = 100.0f;

    // Use distance squared to avoid sqrt
    float _x = dest->x - path->loc->x;
    float _y = dest->y - path->loc->y;
    float dist2 = _x * _x + _y * _y;

    return dist2 + mod;
}

void _add_open(struct PathNode* node)
{
    node->state = OPEN;

    if(!_open_head)
    {
        _open_head = node;
        return;
    }
    else if(node->cost_to_end < _open_head->cost_to_end)
    {
        node->pathlist_next = _open_head;
        _open_head = node;
        return;
    }

    struct PathNode* prev = _open_head;
    while(prev->pathlist_next)
    {
        if(prev->pathlist_next && node->cost_to_end < prev->pathlist_next->cost_to_end)
        {
            node->pathlist_next = prev->pathlist_next;
            prev->pathlist_next = node;
            return;
        }

        prev = prev->pathlist_next;
    }

    prev->pathlist_next = node;
    node->pathlist_next = NULL;
}

/**
 * Go through the open linked list and return the PathNode with the lowest score
 */
struct PathNode* _get_best_open(void)
{
    struct PathNode* best = _open_head;
    _open_head = _open_head->pathlist_next;

    best->pathlist_next = NULL;

    return best;
}

/**
 * A* algorithm constructing shortest path
 */
struct PathNode* _find_path(struct Location* start, struct Location* dest, int path_bits)
{
    struct PathNode* global_best = NULL;
    struct PathNode* best_node = NULL;

    while(_open_head)
    {
        best_node = _get_best_open();

        // Set global best node seen so far
        if(!global_best || best_node->cost_to_end < global_best->cost_to_end)
            global_best = best_node;

        // Reached destination, return
        if(best_node->loc->x == dest->x && best_node->loc->y == dest->y)
            return best_node;

        for(int _x = best_node->loc->x - 1; _x < best_node->loc->x + 2; _x++)
        for(int _y = best_node->loc->y - 1; _y < best_node->loc->y + 2; _y++)
        {
            // Check for invalid location
            if((_x == best_node->loc->x && _y == best_node->loc->y) ||
               ((!map_in_bounds(cmap, _x, _y) || !map_is_pathable(cmap, _x, _y, path_bits)) && (_x != dest->x || _y != dest->y)))
                continue;

            struct PathNode* p = cmap->locs[_x][_y].path_node;

            // Check to see if this is a stale node (if it was last visited in a previous turn or a previous pathing request this turn)
            // Set it to a fresh state
            if(p->turn_visited != current_turn_number || p->gen_id != best_node->gen_id)
            {
                p->turn_visited = current_turn_number;
                p->state = UNVISITED;
                p->gen_id = best_node->gen_id;
            }

            if(p->state == CLOSED)
            {
                // Check if this new path is more cost-efficient
                // If it is, we can reassign the path to this closed node
                if(p->cost_from_start > best_node->cost_from_start + 1)
                {
                    p->prev = best_node;
                    p->cost_from_start = best_node->cost_from_start + 1;
                }
            }
            else if(p->state == UNVISITED)
            {
                // This location has not been visited yet
                // Add to open list
                p->pathlist_next = NULL;
                p->next = NULL;
                p->prev = best_node; 
                p->cost_from_start = best_node->cost_from_start + 1;
                p->cost_to_end = _evaluate(p, dest, path_bits);

                _add_open(p);
            }
        }
    }

    return global_best;
}

struct PathNode* new_path_node(struct Location* loc)
{
    struct PathNode* node = (struct PathNode*) malloc(sizeof(struct PathNode));
    node->loc = loc;
    node->pathlist_next = NULL;
    node->next = NULL;
    node->prev = NULL;
    node->cost_to_end = FLT_MAX;
    node->cost_from_start = FLT_MAX;
    node->state = UNVISITED;
    node->gen_id = -1;
    node->turn_visited = -1;

    return node;
}

/**
 * Returns the next location in the shortest path from start to dest
 */
struct Location* next_path_loc(struct Location* start, struct Location* dest, int path_bits)
{
    _open_head = NULL;

    struct PathNode* node = start->path_node;
    node->pathlist_next = NULL;
    node->prev = NULL;
    node->next = NULL;
    node->cost_from_start = 0.0f;
    node->cost_to_end = _evaluate(node, dest, path_bits);
    node->turn_visited = current_turn_number;
    node->gen_id = ++current_path_gen_id;

    _add_open(node);

    struct Location* ret = NULL;
    if((node = _find_path(start, dest, path_bits)) != NULL)
    {
        ret = _get_first_path_node(node)->loc;
    }

    return ret;
}
