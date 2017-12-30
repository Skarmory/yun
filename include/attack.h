#ifndef attack_h
#define attack_h

#define W_LONGSWORD 0
#define W_CLAWS     1

struct Attack
{
    int num_dice;
    int sides_per_die;
};

struct Weapon
{
    char* name;
    struct Attack attk[1];
};


#endif
