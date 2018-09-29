#ifndef EQUIP_H
#define EQUIP_H

struct Weapon;
struct Armour;

struct Equipment
{
    struct Weapon* main_hand;
    struct Weapon* off_hand;  // This will be null unless dual wielding
};

struct Equipment* new_equipment(void);
void free_equipment(struct Equipment* equipment);

#endif
