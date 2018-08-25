#include "path.h"
#include "util.h"
#include "log.h"
#include <ncurses.h>
#include <stdlib.h>
#include "math.h"
#include "map.h"

struct PathNode* _open_head;
struct PathNode* _open_tail;
struct PathNode* _closed_head;
struct PathNode* _closed_tail;

extern struct PathNode* get_first_path_node(struct PathNode* node);

void __print_node(char* msg, struct PathNode* path)
{
    char _msg[256];
    sprintf(_msg, "%s %d %d", msg, path->loc->x, path->loc->y);
    logmsg(_msg, DEBUG);
}

float _evaluate(struct PathNode* path, struct Location* dest)
{
    // Special handling for the destination
    if(path->loc->x == dest->x && path->loc->y == dest->y)
        return -INFINITY;

    // If somehow this location is not valid, assign infinity to the cost
    if(!valid_move(path->loc->x, path->loc->y))
        return INFINITY;

    // Use distance squared to avoid sqrt
    float _x = dest->x - path->loc->x;
    float _y = dest->y - path->loc->y;
    float dist2 = _x * _x + _y * _y;
    
    return dist2;
}

struct PathNode* _new_path_node(struct Location* loc, struct Location* start, struct Location* dest)
{
    struct PathNode* p = (struct PathNode*)malloc(sizeof(struct PathNode));
    p->pathlist_next = NULL;
    p->prev = NULL;
    p->loc = loc;
    p->cost_to_end = _evaluate(p, dest);
    return p;
}

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

void __log_open(void)
{
    struct PathNode* tmp = _open_head;

    if(!tmp)
        logmsg("No open nodes", DEBUG);

    while(tmp)
    {
        __print_node("open", tmp);
        tmp = tmp->pathlist_next;
    }
}

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


void __log_closed(void)
{
    struct PathNode* tmp = _closed_head;

    if(!tmp)
        logmsg("No closed nodes", DEBUG);

    while(tmp)
    {
        __print_node("closed", tmp);

        tmp = tmp->pathlist_next;
    }
}

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

struct PathNode* _get_open(void)
{
    struct PathNode* tmp = _open_head;
    if(!tmp)
        return NULL;

    _open_head = _open_head->pathlist_next;

    if(!_open_head)
        _open_tail = NULL;

    return tmp;
}

// Go through the open linked list and return the PathNode with the lowest score
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

/*
 * A* algorithm constructing shortest path
 */
struct PathNode* _find_path(struct Location* start, struct Location* dest)
{
    while(_open_head)
    {
        struct PathNode* best_node = _get_best_open();
        
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
            // valid_move returns false if something is on the location, but we need the player's location 
            if(!valid_move(loc->x, loc->y) && (loc->x != dest->x || loc->y != dest->y))
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
                p = _new_path_node((*neighbours)[i], start, dest);
                p->prev = best_node; 
                p->cost_from_start = best_node->cost_from_start + 1;
                _add_open(p);
            }
        }

        free(*neighbours);
        free(neighbours);
    }

    return NULL;
}

/* Free the two path node lists */
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

/* Returns the next location in the shortest path from start to dest */
struct Location* next_path_loc(struct Location* start, struct Location* dest)
{
    _open_head = NULL;
    _open_tail = NULL;
    _closed_head = NULL;
    _closed_tail = NULL;

    struct PathNode* node = _new_path_node(start, start, dest);
    struct Location* ret = NULL;

    node->cost_from_start = 0.0f;
    _add_open(node);

    if((node = _find_path(start, dest)) != NULL)
    {
        ret = get_first_path_node(node)->loc;
    }

//    _debug_draw_path();
    _free_path_lists();

    return ret;
}
