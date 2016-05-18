#ifndef NPC_H
#define NPC_H

#include "dice.h"
#include <string>

class npc : public character
{
 private:
  void choose_traits();
  void place_monster(cell_t *cell_arr, room_t *room_arr, int num_room);
 public:
  std::string *name, *description;
  char symbol;
  int traits, seen_x, seen_y, color, speed;
  npc(cell_t *cell_arr, room_t *room_arr, int num_room, int seq);
  gamestate_t take_turn(heap_t *h, cell_t *cell_arr, path_node_t *t_map,
		      path_node_t *n_t_map, character **c_arr, int t_char);
  char print();
  int attack(int hitpoints);
  
};

int line_of_sight(npc *monster, character *player, cell_t *cell_arr);

#endif
