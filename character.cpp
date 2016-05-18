#include <iostream>

#include "structures.h"
#include "character.h"
#include "dungeon_generator.h"
#include "path_finder.h"
#include "gui.h"
#include "heap.h"
#include "npc.h"
#include "pc.h"

//character comparator function for maintaining the
//turn queue
int character_cmp(const void *key, const void *with)
{
  character *k = (character *) key;
  character *w = (character *) with;

  if(k->turn == w->turn)
    {
      return k->sequence - w->sequence;
    }

  return k->turn - w->turn;
}

void initialize_characters(heap_t *h, cell_t *cell_arr,
			   character **c_arr, room_t *room_arr,
			   int t_char, int num_room, dice *damage)
{
  int i;

  heap_init(h, character_cmp, 0);
  
  *(c_arr + 0) = new pc(cell_arr, room_arr, num_room, 0, damage);

  heap_insert(h, *(c_arr + 0));

  for(i = 1; i < t_char; i++)
    {
      *(c_arr + i) = new npc(cell_arr, room_arr, num_room, i); 

      heap_insert(h, *(c_arr + i));
    }
}

void new_characters(heap_t *h, cell_t *cell_arr,
		    character **c_arr, room_t *room_arr,
		    int t_char, int num_room)
{
  int i;

  for(i = 1; i < t_char; i++)
    {
      delete (character *)(*(c_arr +i));
    }

  heap_delete(h);
  
  heap_init(h, character_cmp, 0);

  ((pc *)(*c_arr))->place_pc(cell_arr, room_arr, num_room);

  heap_insert(h, *(c_arr + 0));

  for(i = 1; i < t_char; i++)
    {
      *(c_arr + i) = new npc(cell_arr, room_arr, num_room, i); 

      heap_insert(h, *(c_arr + i));
    }
}

void delete_characters(heap_t *h, character **c_arr, int t_char)
{
  int i;
  character *curr;
  pc *player;

  player = (pc *)(*c_arr);
  player->inv.delete_inventory();
  delete player;

  for(i = 1; i < t_char; i++)
    {
      curr = (character *)(*(c_arr +i));
      delete curr;
    }

  heap_delete(h);
}

gamestate_t take_turn(heap_t *h, cell_t *cell_arr, path_node_t *t_map,
		      path_node_t *n_t_map, character **c_arr, int t_char)
{
  character *c_turn = (character *)heap_remove_min(h);

  return c_turn->take_turn(h, cell_arr, t_map, n_t_map, c_arr, t_char);
}


