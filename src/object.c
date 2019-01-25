#include "object.h"

#include "armour.h"
#include "log.h"
#include "weapon.h"

#include <stdlib.h>

void free_obj(struct Object* obj)
{
    if(obj->objtype_ptr.weapon)
    {
        switch(obj->objtype)
        {
           case WEAPON: free_weapon(obj->objtype_ptr.weapon); return;
           case ARMOUR: free_armour(obj->objtype_ptr.armour); return;
        }
    }
}
