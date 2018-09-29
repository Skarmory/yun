#include "path.h"

#include "log.h"
#include "map.h"
#include "util.h"

#include <math.h>
#include <ncurses.h>
#include <stdlib.h>

struct PathNode* _open_head;
struct PathNode* _open_tail;
struct PathNode* _closed_head;
struct PathNode* _closed_tail;

extern struct PathNode* get_first_path_node(struct PathNode* node);

//TODO: Create array of path nodes or add path nodes to struct Location

/**
 * Debug print method
 */
void __print_node(char* msg, struct PathNode* path)
{
    char _msg[256];
    sprintf(_msg, "%s %d %d", msg, path->loc->x, path->loc->y);
    log_msg(_msg, DEBUG);
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
    if(!valid_move(path->loc->x, path->loc->y, path_bits))
        mod = 100.0f;

    // Use distance squared to avoid sqrt
    float _x = dest->x - path->loc->x;
    float _y = dest->y - path->loc->y;
    float dist2 = _x * _x + _y * _y;

    return dist2 + mod;
}

/**
 * Create a path node with fields initialised to default values
 */
struct PathNode* _new_path_node(struct Location* loc, struct Location* start, struct Location* dest, int path_bits)
{
    struct PathNode* p = (struct PathNode*)malloc(sizeof(struct PathNode));
    p->pathlist_next = NULL;
    p->prev = NULL;
    p->loc = loc;
    p->cost_to_end = _evaluate(p, dest, path_bits);
    return p;
}

/**
 * Check if node is in the closed list
 * TODO: This is very inefficient.
 */
struct PathNode* _in_closed(struct Location* loc)
{
    struct PathNode* tmp = _closed_head;
    while(tmp)
    {
        if((loc->x == tmp->loc->x) && (loc->y == tmp->loc->y))
            return tmp;

        tmp = tmp->pathlist_next;
    }

    return NULL;
}

/**
 * Check if node is in the open list
 * TODO: This is very inefficient.
 */
struct PathNode* _in_open(struct Location* loc)
{
    struct PathNode* tmp = _open_head;
    while(tmp)
    {
        if((loc->x == tmp->loc->x) && (loc->y == tmp->loc->y))
            return tmp;

        tmp = tmp->pathlist_next;
    }

    return NULL;
}

/**
 * Debug print method
 */
void __log_open(void)
{
    struct PathNode* tmp = _open_head;

    if(!tmp)
        log_msg("No open nodes", DEBUG);

    while(tmp)
    {
        __print_node("open", tmp);
        tmp = tmp->pathlist_next;
    }
}

/**
 * Debug print method
 */
void _add_open(struct PathNode* node)
{
    if(_open_tail)
    {
        _open_tail->pathlist_next = node;
        _open_tail = _open_tail->pathlist_next;
    }
    else if(!_open_head)
    {
        _open_head = node;
        _open_tail = node;
    }

    _open_tail->pathlist_next = NULL;
}

/**
 * Debug print method
 */
void __log_closed(void)
{
    struct PathNode* tmp = _closed_head;

    if(!tmp)
        log_msg("No closed nodes", DEBUG);

    while(tmp)
    {
        __print_node("closed", tmp);

        tmp = tmp->pathlist_next;
    }
}

/**
 * Add node to the closed list
 */
void _add_closed(struct PathNode* node)
{
    if(_closed_tail)
    {
        _closed_tail->pathlist_next = node;
        _closed_tail = _closed_tail->pathlist_next;
    }
    else if(!_closed_head)
    {
        _closed_head = node;
        _closed_tail = node;
    }

    _closed_tail->pathlist_next = NULL;
}

/**
 * Go through the open linked list and return the PathNode with the lowest score
 */
struct PathNode* _get_best_open(void)
{
    struct PathNode *ret, *ret_prev, *prev, *cur;

    // Check for empty or single element list
    if(_open_head == _open_tail)
    {
        ret = _open_head;
        ret->pathlist_next = NULL;
        _open_head = NULL;
        _open_tail = NULL;
        return ret;
    }

    ret_prev = NULL; 
    prev = NULL;
    ret = _open_head; 
    cur = _open_head;

    // Go through and keep track of best value in ret, and the previous node in ret_prev
    // ret_prev tracked so we can relink the list together once we remove ret
    while(cur)
    {
        if(cur->cost_to_end < ret->cost_to_end)
        {
            ret_prev = prev;
            ret = cur;
        }

        prev = cur;
        cur = cur->pathlist_next;
    }

    if(ret == _open_head)
    {
        if(_open_head == _open_tail)
            _open_tail = NULL;

        _open_head = _open_head->pathlist_next;
    }
    else
    {
        ret_prev->pathlist_next = ret->pathlist_next;
        if(ret == _open_tail)
        {
            _open_tail = ret_prev;
        }
    }

    ret->pathlist_next = NULL;

    return ret;
}

/**
 * Draws a visualisation of the path being constructed
 */
void _debug_draw_path(void)
{
    display_map();

    struct PathNode* tmp = _closed_head;
    attron(COLOR_PAIR(1));

    while(tmp)
    {
        mvprintw(tmp->loc->y, tmp->loc->x, "X");

        tmp = tmp->pathlist_next;
    }

    attroff(COLOR_PAIR(1));

    tmp = _open_head;
    attron(COLOR_PAIR(2));

    while(tmp)
    {
        mvprintw(tmp->loc->y,tmp->loc->x, "O");
        tmp = tmp->pathlist_next;
    }

    attroff(COLOR_PAIR(2));

    char c = getch();
    if(c == 'q')
        do_quit();
    refresh();
}

/**
 * A* algorithm constructing shortest path
 */
struct PathNode* _find_path(struct Location* start, struct Location* dest, int path_bits)
{
    struct PathNode* global_best;
    struct PathNode* best_node;

    while(_open_head)
    {
        best_node = _get_best_open();

        // Set global best node seen so far
        if(!global_best || best_node->cost_to_end < global_best->cost_to_end)
            global_best = best_node;

        // Reached destination, return
        if(best_node->loc->x == dest->x && best_node->loc->y == dest->y)
            return best_node;

        _add_closed(best_node);

        struct Location*** neighbours = (struct Location***) malloc(sizeof(struct Location**));
        int ncount = get_neighbours(best_node->loc, neighbours);

        struct Location* loc;
        for(int i = 0; i < ncount; i++)
        {
            loc = (*neighbours)[i];

            // Check for invalid location
            if((!loc_in_bounds(loc->x, loc->y) || !loc_is_pathable(loc->x, loc->y, path_bits))  && (loc->x != dest->x || loc->y != dest->y))
                continue;

            struct PathNode* p;

            if((p = _in_closed(loc)) != NULL)
            {
                // Check if this new path is more cost-efficient
                // If it is, we can reassign the path to this closed node
                if(p->cost_from_start > best_node->cost_from_start + 1)
                {
                    p->prev = best_node;
                    p->cost_from_start = best_node->cost_from_start + 1;
                }

            }
            else if((p = _in_open(loc)) == NULL)
            {
                // This location has not been visited yet
                // Add to open list
                p = _new_path_node((*neighbours)[i], start, dest, path_bits);
                p->prev = best_node; 
                p->cost_from_start = best_node->cost_from_start + 1;
                _add_open(p);
            }
        }

        free(*neighbours);
        free(neighbours);
    }

    return global_best;
}

/**
 * Free the two path node lists
 */
void _free_path_lists(void)
{
    while((_open_tail = _open_head) != NULL)
    {
        _open_head = _open_head->pathlist_next;
        free(_open_tail);
    }

    while((_closed_tail = _closed_head) != NULL)
    {
        _closed_head = _closed_head->pathlist_next;
        free(_closed_tail);
    }
}

/**
 * Returns the next location in the shortest path from start to dest
 */
struct Location* next_path_loc(struct Location* start, struct Location* dest, int path_bits)
{
    _open_head = NULL;
    _open_tail = NULL;
    _closed_head = NULL;
    _closed_tail = NULL;

    struct PathNode* node = _new_path_node(start, start, dest, path_bits);
    struct Location* ret = NULL;

    node->cost_from_start = 0.0f;
    _add_open(node);

    if((node = _find_path(start, dest, path_bits)) != NULL)
    {
        ret = get_first_path_node(node)->loc;
    }

    _free_path_lists();

    return ret;
}
