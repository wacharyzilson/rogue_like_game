#ifndef CHARACTER_H
#define CHARACTER_H

#include "dice.h"

//character class which is super-class of both pc and npc
class character
{
 public:
  int x_pos;
  int y_pos;
  int speed;
  int turn;
  int sequence;
  int hitpoints;
  dice *damage;
  //takes a turn for the given character
  virtual gamestate_t take_turn(heap_t *h, cell_t *cell_arr, path_node_t *t_map,
		      path_node_t *n_t_map, character **c_arr, int t_char) = 0;
  //returns the char that represents the given character
  virtual char print() = 0;
  //attacks a character for given hitpoints, returns amounnt
  //of damage delt in hitpoints
  virtual int attack(int hitpoints) = 0;

};

//populates character array, places characters in the dungeon,
//puts characters in priority queue
void initialize_characters(heap_t *h, cell_t *cell_arr,
			   character **c_arr, room_t *room_arr,
			   int t_char, int num_room, dice *damage);

//removes character, for use when ending game or loading new dungeon
void delete_characters(heap_t *h, character **c_arr, int t_char);

//takes turn for given character
gamestate_t take_turn(heap_t *h, cell_t *cell_arr, path_node_t *t_map,
		      path_node_t *n_t_map, character **c_arr, int t_char);
//for use when going up and down stairs
void new_characters(heap_t *h, cell_t *cell_arr,
		    character **c_arr, room_t *room_arr,
		    int t_char, int num_room);


#endif
