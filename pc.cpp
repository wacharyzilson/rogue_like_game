#include <iostream>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <stdio.h>
#include <string.h>

#include "structures.h"
#include "character.h"
#include "dungeon_generator.h"
#include "path_finder.h"
#include "gui.h"
#include "heap.h"
#include "npc.h"
#include "pc.h"
#include "inventory.h"

static int check_win(character **c_arr, int t_char)
{
  int result, i;
  character *monster;

  result = 0;
  
  for(i = 1; i < t_char; i++)
    {
      monster = *(c_arr + i);
      if(monster->x_pos < 100)
	{
	  result++;
	}
    }

  return result;
}

static void update_all_sight(character **c_arr, cell_t *cell_arr, int t_char)
{
  character *player;
  npc *monster;
  int i, dam;
  
  player = *(c_arr + 0);

  for(i = 1; i < t_char; i++)
    {
      monster = (npc*)(*(c_arr + i));
      if(line_of_sight(monster, player, cell_arr))
	{
	  monster->seen_x = player->x_pos;
	  monster->seen_y = player->y_pos;
	}
    }
}

static gamestate_t move_player(pc *player, cell_t *cell_arr)
{
  input_t move;
  cell_t *curr, *next;
  int n_x, n_y, dam;
  std::stringstream mes;

  curr = cell_arr + (MAP_WIDTH * player->y_pos) + player->x_pos;

  while(1)
    {
    INPUT: move = get_keystroke();

      switch(move)
	{
	case up_left:
	  n_x = -1;
	  n_y = -1;
	  break;
	case up:
	  n_x = 0;
	  n_y = -1;
	  break;
	case up_right:
	  n_x = 1;
	  n_y = -1;
	  break;
	case right:
	  n_x = 1;
	  n_y = 0;
	  break;
	case down_right:
	  n_x = 1;
	  n_y = 1;
	  break;
	case down:
	  n_x = 0;
	  n_y = 1;
	  break;
	case down_left:
	  n_x = -1;
	  n_y = 1;
	  break;
	case left:
	  n_x = -1;
	  n_y = 0;
	  break;
	case d_stair:
	  if(curr->cell_type == down_stair)
	    {
	      return stairs;
	    }
	  goto INPUT;
	case u_stair:
	  if(curr->cell_type == up_stair)
	    {
	      return stairs;
	    }
	  goto INPUT;
	case rest:
	  return normal;
	case save_q:
	  return save_and_quit;
	case m_list:
	  return monster_list;
	case invent:
	  return dis_inventory;
	case drop:
	  return drop_it;
	case equipment:
	  return dis_equipment;
	case equip:
	  return equip_it;
	case dequip:
	  return dequip_it;
	case expunge:
	  return expunge_it;
	case inspect:
	  return inspect_it;
	case c_mode:
	  print_map(player->sight_arr);
	  goto INPUT;
	case ranged:
	  if(ranged_attack())
	    {
	      return normal;
	    }
	  goto INPUT;
	default:
	  goto INPUT;
	}

      n_x += player->x_pos;
      n_y += player->y_pos;

      if(n_x < 1 || n_x >=  MAP_WIDTH - 1)
	{
	  continue;
	}
      if(n_y < 1 || n_y >= MAP_HEIGHT - 1)
	{
	  continue;
	}

      break;
    }
  
  next = cell_arr + (MAP_WIDTH * n_y) + n_x; 

  if(next->cell_type == stone)
    {
      next->hardness = 0;
      next->cell_type = corridor;
    }
  
  if(next->character != NULL)
    {
      if(player->inv.get_equipment(0) == NULL)
	{
	  dam += player->damage->roll();
	}
      dam += player->inv.get_attack_damage();

      dam = ((npc *)(next->character))->attack(dam);

      if(next->character->hitpoints <= 0)
	{
	  mes << "You killed " << *((npc *)next->character)->name;
	  mes << " with an attack of " << dam << " hitpoints";
	  add_message(mes.str());
	  ((character *)(next->character))->x_pos = 150;
	  next->character = NULL;
	  player->killed++;
	}
      else
	{
	  mes << "You attacked " << *((npc *)next->character)->name;
	  mes << " for " << dam << " hitpoints";
	  add_message(mes.str());
	}
      return normal;
    }
  player->x_pos = n_x;
  player->y_pos = n_y;
  curr->character = NULL;
  next->character = player;

  if(next->object != NULL)
    {
      next->object = player->inv.pick_up_items(next->object);
    }

  return normal;
}

void pc::init_sight(cell_t *cell_arr)
{
  cell_t *c_s;
  int i, j;

  for(i = 0; i < MAP_HEIGHT; i++)
    {
      for(j = 0; j < MAP_WIDTH; j++)
	{
	  c_s = sight_arr + (MAP_WIDTH * i) + j;

	  c_s->character = NULL;
	  c_s->cell_type = stone;
	  c_s->part_of_room = NULL;
	  c_s->hardness = 0;
	  c_s->object = NULL;
	}
    }
  
}

void pc::update_sight(cell_t *cell_arr)
{
  cell_t *c_s, *c_d;
  int i, j;

  for(i = y_pos - 3; i <= y_pos + 3; i++)
    {
      for(j = x_pos - 3; j <= x_pos + 3; j++)
	{
	  if(i < 0 || i >= MAP_HEIGHT)
	    {
	      continue;
	    }
	  if(j < 0 || j >= MAP_WIDTH)
	    {
	      continue;
	    }

	  c_s = sight_arr + (MAP_WIDTH * i) + j;
	  c_d = cell_arr + (MAP_WIDTH * i) + j;

	  c_s->character = c_d->character;
	  c_s->cell_type = c_d->cell_type;
	  c_s->object = c_d->object;
	}
    }
}

gamestate_t pc::take_turn(heap_t *h, cell_t *cell_arr, path_node_t *t_map,
			  path_node_t *n_t_map, character **c_arr, int t_char)
{
  int  move_er;
  gamestate_t result;
  path_node_t *c_map;

  move_er = 0;
  result = normal;

  update_sight(cell_arr);
  print_map(sight_arr);
      
  update_all_sight(c_arr, cell_arr, t_char);
  result = move_player(this, cell_arr);

  if(result != normal)
    {
      if(result == monster_list || result == dis_inventory || result == drop_it
	 || dis_equipment || equip_it || dequip_it || expunge_it || inspect_it)
	{
	  heap_insert(h, this);
	}
      return result;
    }
      
  update_tunn_map(t_map, cell_arr);
  update_non_tunn_map(n_t_map, cell_arr);

  update_sight(cell_arr);
  print_map(sight_arr);
      
  if(!check_win(c_arr, t_char))
    {
      print_map(cell_arr);
      return player_won;
    }
      
  turn = turn + (100 / speed);
  heap_insert(h, this);

  return normal;
}

char pc::print()
{
  return '@';
}

void pc::place_pc(cell_t *cell_arr, room_t *room_arr, int num_room)
{
  room_t *room;
  cell_t *cell;

  room = room_arr + 0;

  init_sight(cell_arr);

  x_pos = random_number(room->x, room->x + room->width - 1);
  y_pos = random_number(room->y, room->y + room->height - 1);

  cell = cell_arr + (MAP_WIDTH * y_pos) + x_pos;
  cell->character = this;
}


pc::pc(cell_t *cell_arr, room_t *room_arr, int num_room, int seq, dice *damage)
{
  room_t *room;
  cell_t *cell;

  room = room_arr + 0;

  init_sight(cell_arr);

  x_pos = random_number(room->x, room->x + room->width - 1);
  y_pos = random_number(room->y, room->y + room->height - 1);

  cell = cell_arr + (MAP_WIDTH * y_pos) + x_pos;
  cell->character = this;

  turn = 0;
  sequence = 0;
  speed = 10;
  hitpoints = 100;
  killed = 0;
  this->damage = damage;
}

void pc::update_speed()
{
  speed = 10 + inv.get_speed();
  if(speed < 1)
    {
      speed = 1;
    }
  else if(speed > 99)
    {
      speed = 99;
    }
}

int pc::attack(int hitpoints)
{
  this->hitpoints -= hitpoints;
  
  if(this->hitpoints < 0)
    {
      this->hitpoints = 0;
    }

  return hitpoints;
}


