#include "mon_ai.h"
#include "player.h"
#include "path.h"
#include "attack.h"
#include "log.h"
#include <stdio.h>

/* Make decision for a given mon */
void update_mon_ai(struct Mon* mon)
{
   // Player character is not an ai
   if(mon == you->mon)
       return;

   struct Location* monloc = &cmap->locs[mon->x][mon->y];
   struct Location* youloc = &cmap->locs[you->mon->x][you->mon->y];

   // Find next location closer to the player
   struct Location* next_loc = next_path_loc(monloc, youloc, mon->pathing);

   if(next_loc->x == youloc->x && next_loc->y == youloc->y)
   {
       // Attack the player
       do_mon_attack_player(mon);
   }
   else
   {
       // Move towards the player
       move_mon(mon, next_loc->x, next_loc->y);
   }
}
