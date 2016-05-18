#include <iostream>
#include <cmath>
#include <sstream>

#include "structures.h"
#include "character.h"
#include "dungeon_generator.h"
#include "path_finder.h"
#include "gui.h"
#include "heap.h"
#include "npc.h"

static int sign(int x)
{
  return (x > 0) - (x < 0);
}

int line_of_sight(npc *monster, character *player,
			cell_t *cell_arr)
{
  double delta_x, delta_y, delta_e, error;
  cell_t *c_cell;
  int i, y;

  delta_x = 1.0 * (player->x_pos - monster->x_pos);
  delta_y = 1.0 * (player->y_pos - monster->y_pos);
  error = 0.0;

  if(delta_x == 0.0)
    {
      for(i = monster->y_pos; i <= player->y_pos; i++)
	{
	  c_cell = cell_arr + (MAP_WIDTH * i) + monster->x_pos;

	  if(c_cell->cell_type == stone)
	    {
	      return 0;
	    }
	}
      return 1;
    }

  delta_e = fabs(delta_y / delta_x);
  y = monster->y_pos;

  i = monster->x_pos;
  while(i != player->x_pos)
    {
      c_cell = cell_arr + (MAP_WIDTH * y) + i;

      if(c_cell->cell_type == stone)
	{
	  return 0;
	}

      error += delta_e;
      while(error >= 0.5)
	{
	  c_cell = cell_arr + (MAP_WIDTH * y) + i;
	  
	  if(c_cell->cell_type == stone)
	    {
	      return 0;
	    }
	  y += sign(player->y_pos - monster->y_pos);
	  error -= 1.0;
	}
      i += sign(player->x_pos - monster->x_pos);
    }
  return 1;
}

int update_sight(character *player, npc *monster,
			cell_t *cell_arr)
{
  if(!(line_of_sight(monster, player, cell_arr)))
    {
      return 0;
    }
  monster->seen_x = player->x_pos;
  monster->seen_y = player->y_pos;
  
  return 1;
}

static gamestate_t move_eratic(npc *monster, cell_t *cell_arr,
			       path_node_t *c_path)
{
  int legal_move, n_x, n_y, dam;
  character *temp;
  gamestate_t result;
  cell_t *c_cell, *n_cell;
  std::stringstream mes;

  c_cell = cell_arr + (MAP_WIDTH * monster->y_pos) + monster->x_pos;
  
  legal_move = 0;
  result = normal;

  do{

    n_x = random_number(1, 3) - 2;
    n_y = random_number(1, 3) - 2;

    n_x = monster->x_pos + n_x;
    n_y = monster->y_pos + n_y;

    /*Do not let monster leave the map*/
    
    if(n_y >= MAP_HEIGHT - 1 || n_y < 1)
      {
	continue;
      }
    if(n_x >= MAP_WIDTH - 1 || n_x < 1)
      {
	continue;
      }
    
    n_cell = cell_arr + (MAP_WIDTH * n_y) + n_x;

    if(n_x == monster->x_pos && n_y == monster->y_pos)
      {
	continue;
      }
    else if(monster->traits & IS_TUNNELING || monster->traits & IS_PASS)
      {
	legal_move = 1;
      }
    else if(n_cell->cell_type != stone)
      {
	legal_move = 1;
      }
     
  } while(!legal_move);

  //if it is pass dont worry about tunneling
  if(!(monster->traits & IS_PASS))
    {
      if(n_cell->cell_type == stone)
	{
	  if(monster->traits & IS_TUNNELING)
	    {
	      n_cell->hardness -= 85;
	      if(n_cell->hardness > 0)
		{
		  update_tunn_map(c_path, cell_arr);
		  return normal;
		}
	      else
		{
		  n_cell->hardness = 0;
		  n_cell->cell_type = corridor;
		  update_tunn_map(c_path, cell_arr);
		  result = update_nt_map;
		}
	    }
	  else
	    {
	      return normal;
	    }
	}
    }
  n_cell = cell_arr + (MAP_WIDTH * n_y) + n_x;

  if(n_cell->character != NULL)
     {
       if(((character *)(n_cell->character))->print() == '@')
	 {
	   dam = monster->damage->roll();
	   dam = ((pc *)(n_cell->character))->attack(dam);

	   mes << "Attacked by " << *monster->name;
	   mes << " for " << dam << " hitpoints";
	   add_message(mes.str());

	   if(n_cell->character->hitpoints <= 0)
	     {
	       result = player_died;
	       n_cell->character = NULL;
	     }
	 }
       else
	 {
	   temp = n_cell->character;
	   temp->x_pos = monster->x_pos;
	   temp->y_pos = monster->y_pos;
	   c_cell->character = temp;
	   monster->y_pos = n_y;
	   monster->x_pos = n_x;
	   n_cell->character = monster;
	 }
       return result;
     }
   
   monster->y_pos = n_y;
   monster->x_pos = n_x;
   c_cell->character = NULL;
   n_cell->character = monster;

   return result;

}


typedef gamestate_t (*move_func)(npc *monster, cell_t *cell_arr,
				 path_node_t *c_path, character *player);
/*telepathic, not smart*/
gamestate_t t_ns(npc *monster, cell_t *cell_arr, path_node_t *c_path,
		 character *player)
{
  int n_x, n_y, dam;
  gamestate_t result;
  cell_t *c_cell, *n_cell;
  character *temp;
  std::stringstream mes;

  result = normal;

  c_cell = cell_arr + (MAP_WIDTH * monster->y_pos) + monster->x_pos;

  n_x = player->x_pos - monster->x_pos;
  n_y = player->y_pos - monster->y_pos;

  if(n_x < 0)
    {
      n_x = -1;
    }
  else if(n_x > 0)
    {
      n_x = 1;
    }
  if(n_y < 0)
    {
      n_y = -1;
    }
  else if(n_y > 0)
    {
      n_y = 1;
    }

  n_x += monster->x_pos;
  n_y += monster->y_pos;

  n_cell = cell_arr + (MAP_WIDTH * n_y) + n_x;

  //if pass skip over tunneling
  if(!(monster->traits & IS_PASS))
    {
      if(n_cell->cell_type == stone)
	{
	  if(monster->traits & IS_TUNNELING)
	    {
	      n_cell->hardness -= 85;
	      if(n_cell->hardness > 0)
		{
		  update_tunn_map(c_path, cell_arr);
		  return normal;
		}
	      else
		{
		  n_cell->hardness = 0;
		  n_cell->cell_type = corridor;
		  update_tunn_map(c_path, cell_arr);
		  result = update_nt_map;
		}
	    }
	  else
	    {
	      return normal;
	    }
	}
    }

  n_cell = cell_arr + (MAP_WIDTH * n_y) + n_x;
  
  if(n_cell->character != NULL)
     {
       if(((character *)(n_cell->character))->print() == '@')
	 {
	   dam = monster->damage->roll();
	   dam = ((pc *)(n_cell->character))->attack(dam);

	   mes << "Attacked by " << *monster->name;
	   mes << " for " << dam << " hitpoints";
	   add_message(mes.str());

	   if(n_cell->character->hitpoints <= 0)
	     {
	       result = player_died;
	       n_cell->character = NULL;
	     }
	 }
       else
	 {
	   temp = n_cell->character;
	   temp->x_pos = monster->x_pos;
	   temp->y_pos = monster->y_pos;
	   c_cell->character = temp;
	   monster->y_pos = n_y;
	   monster->x_pos = n_x;
	   n_cell->character = monster;
	 }
       return result;
     }
   
   monster->y_pos = n_y;
   monster->x_pos = n_x;
   c_cell->character = NULL;
   n_cell->character = monster;

   return result;
}

/*not telepathic, not smart*/
gamestate_t nt_ns(npc *monster, cell_t *cell_arr, path_node_t *c_path,
		  character *player)
{
  /*if it can't see the player, do nothing, like a dummy*/
  if(!update_sight(player, monster, cell_arr))
    {
      return normal;
    }

  /*moves the same as t_ns monster*/
  return t_ns(monster, cell_arr, c_path, player); 
}

/*telepathic, smart*/
gamestate_t t_s(npc *monster, cell_t *cell_arr, path_node_t *c_path,
		character *player)
{
  int n_x, n_y, i, j, min, dam;
  gamestate_t result;
  double dist_a, dist_b;
  cell_t *c_cell, *n_cell;
  path_node_t *path;
  character *temp;
  std::stringstream mes;
  
  result = normal;

  c_cell = cell_arr + (MAP_WIDTH * monster->y_pos) + monster->x_pos;

  min = 10000;
  
  for(i = monster->y_pos - 1; i < monster->y_pos + 2; i++)
    {
      for(j = monster->x_pos - 1; j < monster->x_pos + 2; j++)
	{
	  if(i == monster->y_pos && j == monster->x_pos)
	    {
	      continue;
	    }

	  path = c_path + (MAP_WIDTH * i) + j;
	  n_cell = cell_arr + (MAP_WIDTH * i) + j;
	  
	  if(path->distance < min)
	    {
	      min = path->distance;
	      n_y = i;
	      n_x = j;
	    }
	  else if(path->distance == min &&
		  n_cell->cell_type == c_cell->cell_type)
	    {
	      
	      dist_a = pow(player->x_pos - n_x, 2);
	      dist_a += pow(player->y_pos - n_y, 2);
	      dist_a = sqrt(dist_a);

	      dist_b = pow(player->x_pos - j, 2);
	      dist_b += pow(player->y_pos - i, 2);
	      dist_b = sqrt(dist_b);

	      if(dist_b < dist_a)
		{
		  min = path->distance;
	          n_y = i;
	          n_x = j;
		}
	    }
	}
    }
   n_cell = cell_arr + (MAP_WIDTH * n_y) + n_x;

   //if pass do not worry about tunneling
   if(!(monster->traits & IS_PASS))
     {
       if(n_cell->cell_type == stone)
	 {
	   n_cell->hardness -= 85;
	   if(n_cell->hardness > 0)
	     {
	       update_tunn_map(c_path, cell_arr);
	       return normal;
	     }
	   else
	     {
	       n_cell->hardness = 0;
	       n_cell->cell_type = corridor;
	       update_tunn_map(c_path, cell_arr);
	       result = update_nt_map;
	     }
	 }
     }

   n_cell = cell_arr + (MAP_WIDTH * n_y) + n_x;
   
   if(n_cell->character != NULL)
     {
       if(((character *)(n_cell->character))->print() == '@')
	 {
	   dam = monster->damage->roll();
	   dam = ((pc *)(n_cell->character))->attack(dam);

	   mes << "Attacked by " << *monster->name;
	   mes << " for " << dam << " hitpoints";
	   add_message(mes.str());

	   if(n_cell->character->hitpoints <= 0)
	     {
	       result = player_died;
	       n_cell->character = NULL;
	     }
	 }
       else
	 {
	   temp = n_cell->character;
	   temp->x_pos = monster->x_pos;
	   temp->y_pos = monster->y_pos;
	   c_cell->character = temp;
	   monster->y_pos = n_y;
	   monster->x_pos = n_x;
	   n_cell->character = monster;
	 }
       return result;
     }
   
   monster->y_pos = n_y;
   monster->x_pos = n_x;
   c_cell->character = NULL;
   n_cell->character = monster;

   return result;
  
}

/*not telepathic, smart*/
gamestate_t nt_s(npc *monster, cell_t *cell_arr, path_node_t *c_path,
		 character *player)
{
  int n_x, n_y, dam;
  gamestate_t result;
  cell_t *c_cell, *n_cell;
  character *temp;
  std::stringstream mes;

  result = normal;

  c_cell = cell_arr + (MAP_WIDTH * monster->y_pos) + monster->x_pos;

  /*if it sees player, moves same as telpathic smart*/
  if(update_sight(player, monster, cell_arr))
    {
      return t_s(monster, cell_arr, c_path, player);
    }

  /*If monster is where last seen*/

  if(monster->x_pos == monster->seen_x &&
     monster->y_pos == monster->seen_y)
    {
      monster->seen_x = 150;
    }

  /*if it does not know where the player is, guess*/
  if(monster->seen_x > 100)
    {
      monster->seen_x = random_number(1, MAP_WIDTH - 2);
      monster->seen_y = random_number(1, MAP_HEIGHT - 2);
    }

  n_x = monster->seen_x - monster->x_pos;
  n_y = monster->seen_y - monster->y_pos;

  if(n_x < 0)
    {
      n_x = -1;
    }
  else if(n_x > 0)
    {
      n_x = 1;
    }
  if(n_y < 0)
    {
      n_y = -1;
    }
  else if(n_y > 0)
    {
      n_y = 1;
    }

  n_x += monster->x_pos;
  n_y += monster->y_pos;

  n_cell = cell_arr + (MAP_WIDTH * n_y) + n_x;

  //if pass do not worry about this part of code
  if(!(monster->traits & IS_PASS))
    {
      if(monster->traits & IS_TUNNELING)
	{
     
	  if(n_cell->cell_type == stone)
	    {
	      n_cell->hardness -= 85;
	      if(n_cell->hardness > 0)
		{
		  update_tunn_map(c_path, cell_arr);
		  return normal;
		}
	      else
		{
		  n_cell->hardness = 0;
		  n_cell->cell_type = corridor;
		  update_tunn_map(c_path, cell_arr);
		  result = update_nt_map;
		}
	    }
	}
      else if(n_cell->cell_type == stone)
	{
	  n_cell = cell_arr + (MAP_WIDTH * monster->y_pos) + n_x;
	  if(n_cell->cell_type == stone)
	    {
	      n_x = monster->x_pos;
	    }

	  n_cell = cell_arr + (MAP_WIDTH * n_y) + monster->x_pos;
	  if(n_cell->cell_type == stone)
	    {
	      n_y = monster->y_pos;
	    }

	  /*if cant continue on path, guess again*/
	  if(n_y == monster->y_pos && n_x == monster->x_pos)
	    {
	      monster->seen_x = random_number(1, MAP_WIDTH - 2);
	      monster->seen_y = random_number(1, MAP_HEIGHT - 2);
	    }
	}
    }

  n_cell = cell_arr + (MAP_WIDTH * n_y) + n_x;

  if(n_cell->character != NULL)
     {
       if(((character *)(n_cell->character))->print() == '@')
	 {
	   dam = monster->damage->roll();
	   dam = ((pc *)(n_cell->character))->attack(dam);

	   mes << "Attacked by " << *monster->name;
	   mes << " for " << dam << " hitpoints";
	   add_message(mes.str());

	   if(n_cell->character->hitpoints <= 0)
	     {
	       result = player_died;
	       n_cell->character = NULL;
	     }
	 }
       else
	 {
	   temp = n_cell->character;
	   temp->x_pos = monster->x_pos;
	   temp->y_pos = monster->y_pos;
	   c_cell->character = temp;
	   monster->y_pos = n_y;
	   monster->x_pos = n_x;
	   n_cell->character = monster;
	 }
       return result;
     }

   
  monster->y_pos = n_y;
  monster->x_pos = n_x;
  c_cell->character = NULL;
  n_cell->character = monster;

  return result;
}



move_func move_table[] = {
  nt_ns,
  nt_s,
  t_ns,
  t_s,
};


gamestate_t npc::take_turn(heap_t *h, cell_t *cell_arr, path_node_t *t_map,
			   path_node_t *n_t_map, character **c_arr, int t_char)
{
  int move_er;
  gamestate_t result;
  path_node_t *c_map;
  character *player;

  player = *(c_arr + 0);
  result = normal;
  move_er = 0;
  
  if(x_pos > 100)
	{
	  return normal;
	}

      if(traits & IS_TUNNELING || traits & IS_PASS)
	{
	  c_map = t_map;
	}
      else
	{
	  c_map = n_t_map;
	}

      if(traits & IS_ERATIC)
	{
	  move_er = random_number(1 , 100) > 50;
	}
      
      if(move_er)
	{
	  result = move_eratic(this, cell_arr, c_map);
	}
      else
	{

	  result = move_table[traits % 4](this, cell_arr, c_map,
	  					   player);
	}


      if(result == player_died)
	{
	  print_map(cell_arr);
	  return player_died;
	}
      else if(result == update_nt_map)
	{
	  update_non_tunn_map(n_t_map, cell_arr);
	}
  turn = turn + (100 / speed);
  heap_insert(h, this);

  return normal;
}

void npc::place_monster(cell_t *cell_arr, room_t *room_arr, int num_room)
{
  int room_i, x, y, unoc;
  room_t *room;
  cell_t *cell;

  unoc = 0;

  while(!unoc)
    {
     
      room_i = random_number(1 , num_room - 1);
	
      room = room_arr + room_i;

      x = random_number(room->x, room->x + room->width - 1);
      y = random_number(room->y, room->y + room->height - 1);

      cell = cell_arr + (MAP_WIDTH * y) + x;

      if(cell->character == NULL){
	unoc = 1;
      }
    }
  x_pos = x;
  y_pos = y;
  cell->character = this;
}

void npc::choose_traits()
{
  if(random_number(1, 100) > 50)
    {
      traits = traits | IS_ERATIC;
    }
  if(random_number(1, 100) > 50)
    {
      traits = traits | IS_TUNNELING;
    }
  if(random_number(1, 100) > 50)
    {
      traits = traits | IS_TELEPATHIC;
    }
  if(random_number(1, 100) > 50)
    {
      traits = traits | IS_SMART;
    }
}

char npc::print()
{
  return symbol;
}

npc::npc(cell_t *cell_arr, room_t *room_arr, int num_room, int seq)
{
  traits = 0;
  choose_traits();

  turn = 0;
  seen_x = 150;
  seen_y = 150;
  sequence = seq;

  speed = random_number(5, 20);

  place_monster(cell_arr, room_arr, num_room);
}

int npc::attack(int hitpoints)
{
  this->hitpoints -= hitpoints;
  return hitpoints;
}



