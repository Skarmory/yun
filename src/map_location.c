#include "map_location.h"

/**
 * Add object to given map location
 */
bool loc_add_obj(struct MapLocation* loc, struct Object* obj)
{
    // push object onto location object linked list
    list_add(&loc->obj_list, obj);

    return true;
}

/**
 * Remove object from given map location
 */
bool loc_rm_obj(struct MapLocation* loc, struct Object* obj)
{
    ListNode* node = list_find(&loc->obj_list, obj);
    if(node)
    {
        list_rm(&loc->obj_list, node);
        return true;
    }
    return false;
}

//TODO: This is terrible, only a quick hack for now
bool loc_blocks_sight(struct MapLocation* loc)
{
    return loc->symbol.sym == ' ' || loc->symbol.sym == '|' || loc->symbol.sym == '-';
}
