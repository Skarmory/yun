#include "obj_weapon.h"

#include "list.h"
#include "log.h"
#include "object.h"
#include "symbol.h"

#include <stdlib.h>
#include <string.h>

struct WeaponBase* g_weapon_base = NULL;
int g_weapon_base_count = 0;

struct Weapon* weapon_new(struct WeaponBase* base)
{
    struct Weapon* weapon = malloc(sizeof(struct Weapon));
    weapon->base = base;

    weapon->obj = malloc(sizeof(struct Object));
    weapon->obj->name = base->name;
    weapon->obj->desc = base->desc;

    switch(base->class)
    {
        case WEAPON_CLASS_SWORD_1H:
        case WEAPON_CLASS_SWORD_2H:
        case WEAPON_CLASS_MACE_1H:
        case WEAPON_CLASS_MACE_2H:
        case WEAPON_CLASS_DAGGER:
        case WEAPON_CLASS_FIST:
        case WEAPON_CLASS_GUN:
        case WEAPON_CLASS_AXE_1H:
        case WEAPON_CLASS_AXE_2H:
            weapon->obj->symbol = &g_symbol_weapon_metal;
            break;
        case WEAPON_CLASS_BOW:
        case WEAPON_CLASS_STAFF:
        case WEAPON_CLASS_POLEARM:
            weapon->obj->symbol = &g_symbol_weapon_wood;
            break;
        case WEAPON_CLASS_NATURAL:
            // Natural weapons cannot be dropped and should have no symbol
            break;
        case WEAPON_CLASS_NONE:
            log_msg(DEBUG, "Weapon created with WEAPON_CLASS_NONE");
            break;
    }

    weapon->obj->objtype_ptr.weapon = weapon;
    weapon->obj->objtype = OBJ_TYPE_WEAPON;

    return weapon;
}

void weapon_free(struct Weapon* weapon)
{
    free(weapon->obj);
    free(weapon);
}

struct WeaponBase* weapon_base_lookup_by_name(const char* name)
{
    for(int idx = 0; idx < g_weapon_base_count; ++idx)
    {
        if(strcmp(name, g_weapon_base[idx].name) == 0)
            return &g_weapon_base[idx];
    }

    return NULL;
}

enum WeaponClass weapon_class_from_string(const char* name)
{
    if(strcmp(name, "sword1h") == 0) return WEAPON_CLASS_SWORD_1H;
    if(strcmp(name, "sword2h") == 0) return WEAPON_CLASS_SWORD_2H;
    if(strcmp(name, "mace1h") == 0) return WEAPON_CLASS_MACE_1H;
    if(strcmp(name, "mace2h") == 0) return WEAPON_CLASS_MACE_2H;
    if(strcmp(name, "axe1h") == 0) return WEAPON_CLASS_AXE_1H;
    if(strcmp(name, "axe2h") == 0) return WEAPON_CLASS_AXE_2H;
    if(strcmp(name, "bow") == 0) return WEAPON_CLASS_BOW;
    if(strcmp(name, "gun") == 0) return WEAPON_CLASS_GUN;
    if(strcmp(name, "dagger") == 0) return WEAPON_CLASS_DAGGER;
    if(strcmp(name, "staff") == 0) return WEAPON_CLASS_STAFF;
    if(strcmp(name, "polearm") == 0) return WEAPON_CLASS_POLEARM;
    if(strcmp(name, "fist") == 0) return WEAPON_CLASS_FIST;
    if(strcmp(name, "natural") == 0) return WEAPON_CLASS_NATURAL;

    return WEAPON_CLASS_NONE;
}
