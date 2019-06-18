#ifndef NAXX_WEAPON_H
#define NAXX_WEAPON_H

#include "mon_attack.h"

#define W_LONGSWORD 0
#define W_CLAWS     1
#define W_FISTS     2

#define W_RANGE_START 0
#define W_RANGE_END   3

struct Object;

enum WeaponClass
{
    WEAPON_CLASS_SWORD_1H,
    WEAPON_CLASS_SWORD_2H,
    WEAPON_CLASS_MACE_1H,
    WEAPON_CLASS_MACE_2H,
    WEAPON_CLASS_AXE_1H,
    WEAPON_CLASS_AXE_2H,
    WEAPON_CLASS_BOW,
    WEAPON_CLASS_GUN,
    WEAPON_CLASS_DAGGER,
    WEAPON_CLASS_STAFF,
    WEAPON_CLASS_POLEARM,
    WEAPON_CLASS_FIST,
    WEAPON_CLASS_NATURAL,
    WEAPON_CLASS_NONE
};

struct WeaponBase
{
    char name[32];
    char desc[256];
    enum WeaponClass class;
    struct Attack attk[1];
};

/**
 * Struct containing data about weaponry
 */
struct Weapon
{
    struct WeaponBase* base;
    struct Object* obj;
};

struct Weapon* weapon_new(struct WeaponBase* base);
void weapon_free(struct Weapon* weapon);

struct WeaponBase* weapon_base_lookup_by_name(const char* name);
enum WeaponClass weapon_class_from_string(const char* name);

extern struct WeaponBase* g_weapon_base;
extern int g_weapon_base_count;

#endif
