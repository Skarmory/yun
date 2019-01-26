#ifndef PATH_H
#define PATH_H

struct Location;

/**
 * A pathing node.
 *
 * Contains costs of getting to this node from start position and estimated cost of getting to dest from here.
 * Used to build a linked list of path nodes when doing pathing.
 */
struct PathNode
{
    struct Location* loc;
    struct PathNode* pathlist_next;
    struct PathNode* next;
    struct PathNode* prev;
    float cost_to_end;
    float cost_from_start;
};

struct PathNode* new_path_node(struct Location* loc);
struct Location* next_path_loc(struct Location* start, struct Location* dest, int path_bits);

#endif
