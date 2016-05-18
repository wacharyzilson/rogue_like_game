#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <ncurses.h>
#include <vector>
#include <iostream>

#include "structures.h"
#include "dungeon_generator.h"
#include "save_load.h"
#include "path_finder.h"
#include "character.h"
#include "gui.h"
#include "descriptions.h"
#include "entity_factory.h"
#include "object.h"
#include "pc.h"
#include "dice.h"

int main(int argc, char *argv[])
{
  dungeon_t dungeon;
  cell_t *cell_array;
  room_t *room_array;
  path_node_t *t_path, *n_t_path;
  heap_t *p_queue;
  dice *player_dice;
  character **c_array;
  gamestate_t gamestate;
  int number_of_rooms, total_characters;
  int load_m = 0;
  int save_m = 0;
  int nummon_m = 0;
  int i = 1;
  int seed = 1456278206;

  //read in monster defs

  memset(&dungeon, 0, sizeof(dungeon));
  
  parse_descriptions(&dungeon);

  if(dungeon.monster_descriptions.size() < 1 ||
     dungeon.object_descriptions.size() < 1)
    {
      std::cout << "Not enough monster and/or object descriptions to "  
		<< "start the game"
		<< std::endl;
      return 1;
    }
  

  //end reading monster defs

  total_characters = 11;
  gamestate = normal;

  seed = time(NULL);
  srand(seed);
  /*
  printf("Seed %d\n", seed);
  */
  if(argc > 4)
    {
      printf("Error: too many arguments\n");
      return -1;
    }
  for(; i < argc; i++)
    {
      if(!strcmp(argv[i], "--load"))
	{
	  if(!load_m)
	    {
	      load_m = 1;
	    }
	  else
	    {
	      printf("Error: load parameter used twice\n");
	      return -1;
	    }
	}
      else if(!strcmp(argv[i], "--save"))
	{
	  if(!save_m)
	    {
	      save_m = 1;
	    }
	  else
	    {
	      printf("Error: save parameter used twice\n");
	      return -1;
	    }
	}
      
      else if(strstr(argv[i], "--nummon") != NULL)
	{
	  if(!nummon_m)
	    {
	      nummon_m = 1;
	    }
	  else
	    {
	      printf("Error: save parameter used twice\n");
	      return -1;
	    }

	  if(argv[i][8] == '=')
	    {
	      total_characters = atoi(&argv[i][9]);
	      total_characters++;
	    }
	  else
	    {
	      printf("Wrong argument format for --nummon\n");
	      return -1;
	    }
	}
      else
	{
	  printf("Error: wrong argument format\n");
	  return -1;
	}
    }

  if(total_characters < 1)
    {
      printf("Error: wrong argument format for --nummon\n");
      return -1;
    }
  

  t_path = (path_node_t *)malloc(MAP_WIDTH * MAP_HEIGHT * sizeof(*t_path));
  n_t_path = (path_node_t *)malloc(MAP_WIDTH * MAP_HEIGHT * sizeof(*n_t_path));
  cell_array = (cell_t *)malloc(MAP_WIDTH * MAP_HEIGHT * sizeof(*cell_array));
   
  if(load_m)
    {
      if(load_number_of_rooms(&number_of_rooms) == -1)
	{
	  printf("File could not be found\n");
	  return -1;
	}
      room_array = (room_t *)malloc(number_of_rooms * sizeof(*room_array));
      load(cell_array, room_array, &number_of_rooms);
    }
  else
    {
      room_array = (room_t *)malloc(10 * sizeof(*room_array));
      generate_dungeon(cell_array, room_array, &number_of_rooms);     
    }

  if(save_m)
    {
      save(cell_array, room_array, &number_of_rooms);
    }

  player_dice = new dice(0, 1, 4);
  
  c_array = (character **)malloc(total_characters * sizeof(*c_array));
  p_queue = (heap_t *)malloc(sizeof(*p_queue));

  initialize_characters(p_queue, cell_array, c_array, room_array,
			total_characters , number_of_rooms, player_dice);
  build_npcs(&dungeon, c_array, total_characters);

  place_objects(&dungeon, cell_array, room_array, number_of_rooms);

  gui_init(&((pc*)(*c_array))->inv, (pc*)(*c_array), cell_array);

  if(total_characters == 1)
    {
      print_map(cell_array);
      display_win();
    }
  else
    {
      while(1)
	{
	  gamestate = take_turn(p_queue, cell_array, t_path, n_t_path, c_array,
				total_characters);
	  
	  if(gamestate == player_died)
	    {
	      display_lose();
	      break;
	    }
	  else if(gamestate == player_won)
	    {
	      display_win();
	      break;
	    }
	  else if(gamestate == stairs)
	    {
	      delete_objects(cell_array);
	      generate_dungeon(cell_array, room_array, &number_of_rooms);
	      new_characters(p_queue, cell_array, c_array, room_array,
			     total_characters , number_of_rooms);
	      build_npcs(&dungeon, c_array, total_characters);
	      place_objects(&dungeon, cell_array, room_array, number_of_rooms);
	    }
	  else if(gamestate == save_and_quit)
	    { 
	      break;
	    }
	  else if(gamestate == monster_list)
	    {
	      display_monster_list(c_array, total_characters);
	    }
	  else if(gamestate == dis_inventory)
	    {
	      display_inventory();
	    }
	  else if(gamestate == dis_equipment)
	    {
	      display_equipment();
	    }
	  
	}
    }

  gui_dinit();
  delete_characters(p_queue, c_array, total_characters);
  delete_objects(cell_array);
  delete player_dice;
  
  destroy_descriptions(&dungeon);
  
  free(c_array);
  free(p_queue);
  free(t_path);
  free(n_t_path);
  free(room_array);
  free(cell_array);
  
  return 0; 
   
}
