#ifndef INVENTORY_H
#define INVENTORY_H

#include "object.h"

//equipment types
typedef enum pc_equipment{
  pc_equip_weapon,
  pc_equip_offhand,
  pc_equip_ranged,
  pc_equip_armor,
  pc_equip_helmet,
  pc_equip_cloak,
  pc_equip_gloves,
  pc_equip_boots,
  pc_equip_amulet,
  pc_equip_light,
  pc_equip_ring_left,
  pc_equip_ring_right,
} pc_equipment_t;

class inventory{
 private:
  int carrying;
  object *equipment[12];
  object *carry[10];
  void sort_carry();
 public:
 inventory() : carrying(0), equipment(), carry()
    {
    }

  void delete_inventory();
  
  inline int  has_space() {return 10 - carrying; };
  inline object *get_carry(int index) {return carry[index]; };
  inline object *get_equipment(int index) {return equipment[index]; };
  void equip(int index_c, int index_e);
  void dequip(int index_e);
  void expunge(int index_c);
  int get_speed();
  int get_attack_damage();
  object *pick_up_items(object *item_stack);
  void remove_from_carry(int index, cell_t *loc);
  int get_value();
};	     

#endif
