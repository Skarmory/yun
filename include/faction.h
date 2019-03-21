#ifndef NAXX_FACTION_H
#define NAXX_FACTION_H

#define FA_NONE     0
#define FA_ALLIANCE 1 
#define FA_HORDE    2
#define FA_SCOURGE  3 

extern char* factions[];

int get_faction_by_race(char race);
char* get_faction_name(int idx);

#endif
