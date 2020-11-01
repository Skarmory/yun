#ifndef YUN_PLAYER_H
#define YUN_PLAYER_H

struct Class;
struct Mon;

#define mon_is_player(m) (m == g_you->mon)

/**
 * Struct containing information about the player
 */
struct Player
{
    char*         name;
    struct Class* cls;
    struct Mon*   mon;
};

void new_player(void);
void free_player(void);
void player_chk_dead(void);

extern struct Player* g_you;

#endif
