#include "armour.h"

#include <stdlib.h>

#include "object.h"
#include "symbol.h"

#define ARMOUR(val, slot, str, agi, in, spi, sta)\
    { val, slot, str, agi, in, spi, sta, NULL }

struct Armour* new_armour(int armour_type)
{
    struct Armour* armour = (struct Armour*) malloc(sizeof(struct Armour));

    armour->strength = armours[armour_type].strength;
    armour->agility = armours[armour_type].agility;
    armour->intelligence = armours[armour_type].intelligence;
    armour->spirit = armours[armour_type].spirit;
    armour->stamina = armours[armour_type].stamina;

    armour->obj = (struct Object*) malloc(sizeof(struct Object));
    armour->obj->name = armour_names[armour_type];
    armour->obj->desc = armour_descs[armour_type];
    armour->obj->symbol = &symbols[SYM_ARMOUR_OFF + armour_type];
    armour->obj->objtype_ptr.armour = armour;
    armour->obj->objtype = OBJ_TYPE_ARMOUR;
    list_entry_init(&armour->obj->obj_list_entry);

    return armour;
}

void free_armour(struct Armour* armour)
{
    free(armour->obj);
    free(armour);
}

//TODO: Refactor to load this info from files rather than be hardcoded
struct Armour armours[] = 
{
    ARMOUR(0, EQUIP_SLOT_CHEST, 0, 0, 0, 0, 0),

    // Cloth
    ARMOUR(2, EQUIP_SLOT_HEAD,      0, 1, 7, 3, 6), // Magister's Crown
    ARMOUR(1, EQUIP_SLOT_SHOULDERS, 0, 2, 4, 4, 4), // Magister's Mantle
    ARMOUR(3, EQUIP_SLOT_CHEST,     0, 1, 3, 4, 7), // Magister's Robe
    ARMOUR(1, EQUIP_SLOT_HANDS,     0, 2, 6, 1, 3), // Magister's Gloves
    ARMOUR(1, EQUIP_SLOT_LEGS,      0, 2, 4, 5, 6), // Magister's Leggings
    ARMOUR(1, EQUIP_SLOT_FEET,      0, 2, 6, 1, 4), // Magister's Boots

    // Leather
    ARMOUR(4, EQUIP_SLOT_HEAD,      1, 4, 0, 2, 6), // Shadowcraft Cap
    ARMOUR(2, EQUIP_SLOT_SHOULDERS, 1, 6, 0, 2, 4), // Shadowcraft Spaulders
    ARMOUR(5, EQUIP_SLOT_CHEST,     4, 3, 0, 2, 7), // Shadowcraft Tunic
    ARMOUR(2, EQUIP_SLOT_HANDS,     5, 6, 0, 1, 3), // Shadowcraft Gloves
    ARMOUR(3, EQUIP_SLOT_LEGS,      4, 4, 0, 2, 6), // Shadowcraft Pants
    ARMOUR(2, EQUIP_SLOT_FEET,      3, 7, 0, 1, 4), // Shadowcraft Boots

    // Mail
    ARMOUR(6, EQUIP_SLOT_HEAD,      0, 4, 5, 2, 6), // Beaststalker's Cap
    ARMOUR(4, EQUIP_SLOT_SHOULDERS, 0, 6, 4, 2, 4), // Beaststalker's Spaulders
    ARMOUR(7, EQUIP_SLOT_CHEST,     0, 3, 3, 2, 7), // Beaststalker's Tunic
    ARMOUR(4, EQUIP_SLOT_HANDS,     0, 6, 1, 1, 3), // Beaststalker's Gloves
    ARMOUR(5, EQUIP_SLOT_LEGS,      0, 4, 4, 2, 6), // Beaststalker's Pants
    ARMOUR(4, EQUIP_SLOT_FEET,      0, 7, 1, 1, 4), // Beaststalker's Boots

    // Plate
    ARMOUR(8, EQUIP_SLOT_HEAD,      3, 1, 0, 2, 6), // Helm of Valor
    ARMOUR(6, EQUIP_SLOT_SHOULDERS, 6, 3, 0, 2, 4), // Spaulders of Valor
    ARMOUR(9, EQUIP_SLOT_CHEST,     4, 1, 0, 2, 7), // Breastplate of Valor
    ARMOUR(6, EQUIP_SLOT_HANDS,     7, 4, 0, 1, 3), // Gauntlets of Valor
    ARMOUR(7, EQUIP_SLOT_LEGS,      4, 4, 0, 2, 6), // Legplates of Valor
    ARMOUR(6, EQUIP_SLOT_FEET,      6, 5, 0, 1, 4), // Boots of Valor
};

char* armour_names[] =
{
    "Unarmoured",

    "Magister's Crown",
    "Magister's Mantle",
    "Magister's Robe",
    "Magister's Gloves",
    "Magister's Leggings",
    "Magister's Boots",

    "Shadowcraft Cap",
    "Shadowcraft Spaulders",
    "Shadowcraft Tunic",
    "Shadowcraft Gloves",
    "Shadowcraft Pants",
    "Shadowcraft Boots",

    "Beaststalker's Cap",
    "Beaststalker's Spaulders",
    "Beaststalker's Tunic",
    "Beaststalker's Gloves",
    "Beaststalker's Pants",
    "Beaststalker's Boots",

    "Helm of Valor",
    "Spaulders of Valor",
    "Breastplate of Valor",
    "Gauntlets of Valor",
    "Legplates of Valor",
    "Boots of Valor"
};

char* armour_descs[] =
{
    "An unarmoured fighter relies on their dexterity and agility to evade strikes rather than enduring through their opponent's attacks. However, a single failure could be fatal.",
    "Red and purple regalia favoured by the Mages of the Kirin Tor.",
    "Red and purple regalia favoured by the Mages of the Kirin Tor.",
    "Red and purple regalia favoured by the Mages of the Kirin Tor.",
    "Red and purple regalia favoured by the Mages of the Kirin Tor.",
    "Red and purple regalia favoured by the Mages of the Kirin Tor.",
    "Red and purple regalia favoured by the Mages of the Kirin Tor.",
    "Black leather armour designed for stealth in dark places.",
    "Black leather armour designed for stealth in dark places.",
    "Black leather armour designed for stealth in dark places.",
    "Black leather armour designed for stealth in dark places.",
    "Black leather armour designed for stealth in dark places.",
    "Black leather armour designed for stealth in dark places.",
    "A golden-brown mail worn by the wandering trackers of Kalimdor. This helm is crafted in the likeness of a wolf's head.",
    "A golden-brown mail worn by the wandering trackers of Kalimdor.",
    "A golden-brown mail worn by the wandering trackers of Kalimdor.",
    "A golden-brown mail worn by the wandering trackers of Kalimdor.",
    "A golden-brown mail worn by the wandering trackers of Kalimdor.",
    "A golden-brown mail worn by the wandering trackers of Kalimdor.",
    "Forged from heavy steel plating and decorated with a trim made of gold.",
    "Forged from heavy steel plating and decorated with a trim made of gold.",
    "Forged from heavy steel plating and decorated with a trim made of gold.",
    "Forged from heavy steel plating and decorated with a trim made of gold.",
    "Forged from heavy steel plating and decorated with a trim made of gold.",
    "Forged from heavy steel plating and decorated with a trim made of gold."
};
