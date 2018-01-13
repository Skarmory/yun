#ifndef STATS_H
#define STATS_H

#include <stdbool.h>

#define STRENGTH     0
#define AGILITY      1
#define INTELLIGENCE 2
#define SPIRIT       3
#define STAMINA      4

#define PSTAT(s, t) you->mon->stats.s.t
#define MSTAT(m, s, t) m->stats.s.t

struct Mon;

struct Strength
{
    int base_strength;
    int strength;
};

struct Agility
{
    int base_agility;
    int agility;
};

struct Intelligence 
{
    int base_intelligence;
    int intelligence;
};

struct Stamina 
{
    int base_stamina;
    int stamina; 
};

struct Spirit
{
    int base_spirit;
    int spirit; 
};

struct Stats
{
    struct Strength strength;
    struct Agility agility;
    struct Intelligence intelligence;
    struct Stamina stamina;
    struct Spirit spirit;
};

void add_stat(struct Mon* mon, int stat, int amount, bool base);

#endif
