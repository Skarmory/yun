#ifndef STATS_H
#define STATS_H

struct Strength
{
    int base_strength;
    int strength;
}

struct Agility
{
    int base_agility;
    int agility;
}

struct Intelligence 
{
    int base_intelligence;
    int intelligence;
}

struct Stamina 
{
    int base_stamina;
    int stamina; 
}

struct Spirit
{
    int base_spirit;
    int spirit; 
}

struct Stats
{
    struct Strength strength;
    struct Agility agility;
    struct Intelligence intelligence;
    struct Stamina stamina;
    struct Spirit spirit;
}

#endif
