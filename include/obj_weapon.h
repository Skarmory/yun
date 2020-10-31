#ifndef YUN_WEAPON_H
#define YUN_WEAPON_H

#include "mon_attack.h"

struct Object;

enum WeaponClass
{
    WEAPON_CLASS_SWORD,
    WEAPON_CLASS_MACE,
    WEAPON_CLASS_AXE,
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
    char id[5];
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

struct WeaponBase* weapon_base_look_up_by_id(const char* id);
enum WeaponClass weapon_class_from_string(const char* name);

extern struct WeaponBase* g_weapon_base;
extern int g_weapon_base_count;

#endif
