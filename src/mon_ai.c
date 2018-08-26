#include "mon_ai.h"
#include "player.h"
#include "path.h"
#include "log.h"
#include <stdio.h>

// Forward declares
void _update_mon(struct Mon* mon);

// Functions

/* Make decision for a given mon */
void _update_mon(struct Mon* mon)
{
   // For now, just move at the player
   struct Location* monloc = &cmap->locs[mon->x][mon->y];
   struct Location* youloc = &cmap->locs[you->mon->x][you->mon->y];

   struct Location* next_loc = next_path_loc(monloc, youloc, mon->pathing);

   move_mon(mon, next_loc->x, next_loc->y); 
}

/* Go through list of monsters on the map and make decisions for their turn */
void update_mons(void)
{
    struct Mon* mon = cmap->monlist;

    while(mon)
    {
       _update_mon(mon); 
       mon = mon->next;
    }
}
