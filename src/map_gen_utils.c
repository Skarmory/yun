//#include "map_gen_utils.h"
//
//#include "map_location.h"
//
//#include <scieppened/core/symbol.h>
//
//#include <stddef.h>
//
//
//void map_gen_util_make_corridor(struct MapLocation* loc)
//{
//    if(loc->terrain == C_NULL_ENTITY_HANDLE)
//    {
//        loc->terrain = entity_create();
//        add_position_component(loc->terrain, loc->x, loc->y);
//        add_gfx_component(loc->terrain, );
//    }
//
//    loc->symbol.sym = '#';
//    //loc->pathing_flags = PATHING_GROUND;
//    //loc->feature = NULL;
//}
//
//void map_gen_util_make_floor(struct MapLocation* loc)
//{
//    loc->symbol.sym = '.';
//    loc->pathing_flags = PATHING_GROUND;
//    loc->feature = NULL;
//}
//
//void map_gen_utils_init(void)
//{
//}
