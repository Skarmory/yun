#include "armour.h"

#define ARMOUR(n, str, agi, in, spi, sta)\
    { n, str, agi, in, spi, sta }

struct Armour armours[] = 
{
    ARMOUR("Unarmoured", 0, 0, 0, 0, 0)
};
