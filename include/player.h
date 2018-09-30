#ifndef PLAYER_H
#define PLAYER_H

struct Class;
struct Race;
struct Mon;

#define mon_is_player(m) (m == you->mon)

/**
 * Struct containing information about the player
 */
struct Player
{
    char* name;
    char* faction;

    struct Class* cls;
    struct Race* race;

    struct Mon* mon;
};

void new_player(void);
void destroy_player(void);
void player_chk_dead(void);

extern struct Player* you;

#endif
