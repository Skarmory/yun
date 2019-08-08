#include "mon_ai.h"

#include "log.h"
#include "map.h"
#include "map_cell.h"
#include "map_location.h"
#include "monster.h"
#include "mon_attack.h"
#include "pathing.h"
#include "player.h"

#include <stdio.h>

/* Make decision for a given mon */
void update_mon_ai(struct Mon* mon)
{
   // Player character is not an ai
   if(mon == you->mon)
       return;

   struct MapCell* mon_cell = map_get_cell_by_world_coord(cmap, mon->x, mon->y);
   struct MapCell* you_cell = map_get_cell_by_world_coord(cmap, you->mon->x, you->mon->y);
   struct MapLocation* monloc = map_cell_get_location(mon_cell, mon->x, mon->y);
   struct MapLocation* youloc = map_cell_get_location(you_cell, you->mon->x, you->mon->y);

   // Find next location closer to the player
   struct MapLocation* next_loc = next_path_loc(monloc, youloc, mon->move_flags);

   if(next_loc->x == youloc->x && next_loc->y == youloc->y)
   {
       // Attack the player
       do_attack_mon_mon(mon, you->mon);
   }
   else
   {
       // Move towards the player
       mon_move(mon, next_loc->x, next_loc->y);
   }
}
