#ifndef PATH_H
#define PATH_H

struct Location;

/**
 * Enum representing whether a pathing node has been visited
 */
enum NodeState
{
    UNVISITED =  0,
    OPEN      =  1,
    CLOSED    = -1
};

/**
 * A pathing node.
 *
 * Contains costs of getting to this node from start position and estimated cost of getting to dest from here.
 * Used to build a linked list of path nodes when doing pathing.
 *
 * loc: Location linked to this path node
 * pathlist_next: Next node in linked list used in internal pathing algorithm
 * next: Next node in generated path
 * prev: Previous node in generated path
 * cost_to_end: Estimated cost of getting to the ending location via this node
 * cost_from_start: Estimated cost of getting to this node from the starting location
 * state: Enum representing whether a node has been visited by the pathing algorithm
 * turn_invalidated: A toggle that indicates that a new turn has been issued
 * gen_id: The id given to the pathing attempt in which this node was last visited
 */
struct PathNode
{
    struct Location* loc;
    struct PathNode* pathlist_next;
    struct PathNode* next;
    struct PathNode* prev;
    float cost_to_end;
    float cost_from_start;
    enum NodeState state;
    int turn_visited;
    int gen_id;
};

struct PathNode* new_path_node(struct Location* loc);
struct Location* next_path_loc(struct Location* start, struct Location* dest, int path_bits);

#endif
