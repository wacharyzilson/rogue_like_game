#ifndef PC_H
#define PC_H

#include "inventory.h"

class pc : public character
{
 public:
  int killed;
  inventory inv;
  cell_t sight_arr[MAP_HEIGHT * MAP_WIDTH];
  void update_sight(cell_t *cell_arr);
  void init_sight(cell_t *cell_arr);
  pc(cell_t *cell_arr, room_t *room_arr, int num_room, int seq, dice *damage);
  gamestate_t take_turn(heap_t *h, cell_t *cell_arr, path_node_t *t_map,
		      path_node_t *n_t_map, character **c_arr, int t_char);
  char print();
  int attack(int hitpoints);

  void place_pc(cell_t *cell_arr, room_t *room_arr, int num_room);
  void update_speed();
  inline int get_score(){return inv.get_value() + (killed * 100); };
};

#endif
