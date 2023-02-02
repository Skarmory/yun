#include "mon_ai.h"

#include "map.h"
#include "map_cell.h"
#include "map_location.h"
#include "monster.h"
#include "mon_attack.h"
#include "movement.h"
#include "pathing.h"
#include "player.h"

#include <scieppend/core/log.h>

#include <stdio.h>

/* Make decision for a given mon */
void update_mon_ai(struct Mon* mon)
{
   // Player character is not an ai
   if(mon == g_you->mon)
   {
       return;
   }

   struct MapLocation* monloc = map_get_location(g_cmap, mon->x, mon->y);
   struct MapLocation* youloc = map_get_location(g_cmap, g_you->mon->x, g_you->mon->y);

   // Find next location closer to the player
   struct MapLocation* next_loc = next_path_loc(monloc, youloc, mon->move_flags);

   if(next_loc->x == youloc->x && next_loc->y == youloc->y)
   {
       // Attack the player
       do_attack_mon_mon(mon, g_you->mon);
   }
   else
   {
       // Move towards the player
       move_mon(mon, next_loc->x, next_loc->y);
   }
}
