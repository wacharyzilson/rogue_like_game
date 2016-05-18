#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "structures.h"
#include "dungeon_generator.h"
#include "heap.h"

//for generating dungeon paths
typedef struct path_node_c{
  int x;
  int y;
  int distance;
  int weight;
  heap_node_t *heap_node;
  struct path_node_c *previous;
} path_node_c_t;

int random_number(int min, int max)
{
  return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

static void add_stairs(cell_t *cell_array, room_t *room_array, int num_rooms)
{
  room_t *c_room;
  cell_t *c_cell;

  u_int8_t u_room, d_room, x, y;
  
  u_room = 0;
  d_room = 0;

  u_room = random_number(1, num_rooms - 1);
  c_room = room_array + u_room;
   
  x = random_number(c_room->x, c_room->x + c_room->width - 1);
  y = random_number(c_room->y, c_room->y + c_room->height - 1);

  c_cell = cell_array + (MAP_WIDTH * y) + x;
  
  c_cell->cell_type = up_stair;

  while(!d_room)
    {
      d_room = random_number(1, num_rooms - 1);
      
      if(d_room == u_room)
	{
	  d_room = 0;
	}
    }
  
  c_room = room_array + d_room;
  
  x = random_number(c_room->x, c_room->x + c_room->width - 1);
  y = random_number(c_room->y, c_room->y + c_room->height - 1);

  c_cell = cell_array + (MAP_WIDTH * y) + x;
  
  c_cell->cell_type = down_stair;
}

static void generate_hardness(cell_t *cell_array)
{
  int i = 0;
  int j = 0;
  cell_t *current;

  for(; i < MAP_HEIGHT; i++)
    {
      for(j = 0; j < MAP_WIDTH; j++)
	{
	  current = cell_array + (MAP_WIDTH * i) + j;
	  current->part_of_room = NULL;
	  current->character = NULL;
	  current->object = NULL;
	  current->cell_type = stone;
	  if(i == 0 || i == MAP_HEIGHT - 1)
	    {
	      current->hardness = 255;
	    }
	  else if(j == 0 || j == MAP_WIDTH - 1)
	    {
	      current->hardness = 255;
	    }
	  else
	    {
	      current->hardness = random_number(1 , 244);
	    }
	}
    }
}


static int check_valid_room(cell_t *cell_array, room_t *room)
{
  int i;
  int j;
  cell_t *current;
  
  for(i = room->y; i < (room->y + room->height); i++)
    {
      for(j = (room->x - 1); j < (room->x + room->width + 1); j++)
	{
	  current = cell_array + (MAP_WIDTH * i) + j;
	  if(current->hardness == 0)
	    {
	      return -1;
	    }
	}
    }
  for(i = room->x; i < (room->x + room->width); i++)
    {
      current = cell_array + (MAP_WIDTH * (room->y - 1)) + i;
      if(current->hardness == 0)
	{
	  return -1;
	}
      current = cell_array + (MAP_WIDTH * (room->y + room->height)) + i;
      if(current->hardness == 0)
	{
	  return -1;
	}
    }
  return 0;
}

static int add_room(cell_t *cell_array, room_t *room)
{
  int tries = 0;
  room->width = random_number(MIN_ROOM_W , MAX_ROOM_W);
  room->height = random_number(MIN_ROOM_H , MAX_ROOM_H);
  room->x = random_number(1, MAP_WIDTH - 1 - room->width);
  room->y = random_number(1 , MAP_HEIGHT - 1 - room->height);
  while(tries < 200)
    {
      if(check_valid_room(cell_array, room) == 0)
	{
	  return 0;
	}
      else
	{
	  room->x = random_number(1, MAP_WIDTH - 1 - room->width);
	  room->y = random_number(1, MAP_HEIGHT - 1 - room->height);
	}
      tries++;
    }
  return -1;
}

static void add_room_to_map(cell_t *cell_array, room_t *cur_room)
{
  
  int i;
  int j;
  cell_t *current;

  for(i = cur_room->y; i < (cur_room->y + cur_room->height); i++)
    {
      for(j = cur_room->x; j < (cur_room->x + cur_room->width); j++)
	{
	  current = cell_array + (MAP_WIDTH * i) + j;
	  current->hardness = 0;
	  current->cell_type = room;
	  current->part_of_room = cur_room;
	}
    }
}

static void generate_rooms(cell_t *cell_array, room_t *room_array, int *total_rooms)
{
  int result;
  room_t *current_room;
  
  while((*total_rooms) < MAX_ROOMS)
    {
      current_room = room_array + (*total_rooms);
      result = add_room(cell_array , current_room);
      if(result == 0)
	{
	  add_room_to_map(cell_array, current_room);
	  (*total_rooms)++;
	}
      else if((*total_rooms) >= MIN_ROOMS)
	{
	  return;
	}
    }
}

//for comparing paths in priority queue
int gen_path_node_cmp(const void *key, const void *with)
{
  path_node_c_t *t_key = (path_node_c_t *) key;
  path_node_c_t *t_with = (path_node_c_t *) with;

  return t_key->distance - t_with->distance;
}

//paths are made by connecting each room in the rooms array
//to the next room in the array. paths are formed by using
//dijkstra's algorithm where the weight of each node
//is based on a randomly generated hardness or is 1 if it
//is a path or room cell
static void initialize_heap(heap_t *h, path_node_c_t *path_arr, cell_t *cell_arr)
{
  int i, j;
  path_node_c_t *curr_p;
  cell_t *curr_c;

  for(i = 0; i < MAP_HEIGHT; i++)
    {
      for(j = 0; j < MAP_WIDTH; j++)
	{
	  curr_c = cell_arr + (MAP_WIDTH * i) + j;
	  curr_p = path_arr + (MAP_WIDTH * i) + j;

	  curr_p->x = j;
	  curr_p->y = i;
	  curr_p->distance = INT_MAX;

	  if(curr_c->hardness == 255)
	    {
	      curr_p->heap_node = NULL;
	      continue;
	    }
	  if(curr_c->hardness == 0)
	    {
	      curr_p->weight = 1;
	    }
	  else if(curr_c->hardness < 85)
	    {
	      curr_p->weight = 2;
	    }
	  else if(curr_c->hardness < 171)
	    {
	      curr_p->weight = 3;
	    }
	  else
	    {
	      curr_p->weight = 4;
	    }

	  curr_p->heap_node = heap_insert(h, curr_p);
	}
    }
}
			    
static void create_corridor(heap_t *h, path_node_c_t *path_arr)
{
  path_node_c_t *curr_p, *curr_n_p;

  while(h->size > 0)
    { 
    curr_p = (path_node_c_t *) heap_remove_min(h);
    curr_p->heap_node = NULL;

    curr_n_p = path_arr + (MAP_WIDTH * (curr_p->y - 1)) + curr_p->x;

    if(curr_n_p->heap_node != NULL)
      {
	if(curr_n_p->distance > curr_p->distance + curr_n_p->weight)
	  {
	    curr_n_p->distance = curr_p->distance + curr_n_p->weight;
	    curr_n_p->previous = curr_p;
	    heap_decrease_key_no_replace(h, curr_n_p->heap_node);
	  }
      }
    curr_n_p = path_arr + (MAP_WIDTH * curr_p->y) + curr_p->x + 1;

    if(curr_n_p->heap_node != NULL)
      {
	if(curr_n_p->distance > curr_p->distance + curr_n_p->weight)
	  {
	    curr_n_p->distance = curr_p->distance + curr_n_p->weight;
	    curr_n_p->previous = curr_p;
	    heap_decrease_key_no_replace(h, curr_n_p->heap_node);
	  }
      }
    curr_n_p = path_arr + (MAP_WIDTH * (curr_p->y + 1)) + curr_p->x;

    if(curr_n_p->heap_node != NULL)
      {
	if(curr_n_p->distance > curr_p->distance + curr_n_p->weight)
	  {
	    curr_n_p->distance = curr_p->distance + curr_n_p->weight;
	    curr_n_p->previous = curr_p;
	    heap_decrease_key_no_replace(h, curr_n_p->heap_node);
	  }
      }
    curr_n_p = path_arr + (MAP_WIDTH * curr_p->y) + curr_p->x - 1;

    if(curr_n_p->heap_node != NULL)
      {
	if(curr_n_p->distance > curr_p->distance + curr_n_p->weight)
	  {
	    curr_n_p->distance = curr_p->distance + curr_n_p->weight;
	    curr_n_p->previous = curr_p;
	    heap_decrease_key_no_replace(h, curr_n_p->heap_node);
	  }
      }
  }
}

static void  place_corridor(path_node_c_t *curr_p, cell_t *cell_arr)
{
  cell_t *curr_c;
  
  while(curr_p != NULL)
    {
      curr_c = cell_arr + (MAP_WIDTH * curr_p->y) + curr_p->x;  

      if(curr_c->cell_type == stone)
	{
	  curr_c->cell_type = corridor;
	  curr_c->hardness = 0;
	}

      curr_p = curr_p->previous;
    }
}

static void connect_rooms(cell_t *cell_arr, room_t *room_arr, int num_room)
{
  int i, x, y;
  path_node_c_t *path_arr, *curr_p;
  heap_t *h;
  room_t *room;

  h = (heap_t *) malloc(sizeof(*h));

  for(i = 1; i < num_room; i++)
    {
      room = room_arr + i;
      x = random_number(room->x, room->x + room->width - 1);
      y = random_number(room->y, room->y + room->height - 1);
   
      heap_init(h, gen_path_node_cmp, NULL);
      path_arr = (path_node_c_t *) malloc(MAP_HEIGHT * MAP_WIDTH *
				  sizeof(*path_arr));
      initialize_heap(h, path_arr, cell_arr);
           
      curr_p = path_arr + (MAP_WIDTH * y) + x;
      curr_p->weight = 1;
      curr_p->distance = 0;
      curr_p->previous = NULL;
      heap_decrease_key_no_replace(h, curr_p->heap_node);

      create_corridor(h, path_arr);

      room = room_arr + i - 1;
      x = random_number(room->x, room->x + room->width - 1);
      y = random_number(room->y, room->y + room->height - 1);
      curr_p = path_arr + (MAP_WIDTH * y) + x;

      place_corridor(curr_p, cell_arr);

      heap_delete(h);
      free(path_arr);
    }
  
  free(h);
}
/*
void print_map(cell_t *cell_array)
{
  cell_t *current;
  
  for(int i = 0; i < MAP_HEIGHT; i++)
    {
      for(int j = 0; j < MAP_WIDTH; j++)
	{
	  current = cell_array + (MAP_WIDTH * i) + j;
	  if(current->character != NULL)
	    {
	      printf("%c", print_character(current->character));
	    }
	  else if(current->cell_type == stone)
	    {
	      printf(" ");
	    }
	  else if(current->cell_type == room)
	    {
	      printf(".");
	    }
	  else if(current->cell_type == corridor)
	    {
	      printf("#");
	    }
	}
      printf("\n");
    }
} 
*/

int generate_dungeon(cell_t *cell_array, room_t *room_array,
		     int *number_of_rooms)
{ 
  *number_of_rooms = 0;
  
  generate_hardness(cell_array);
  
  generate_rooms(cell_array, room_array, number_of_rooms);

  connect_rooms(cell_array, room_array, *number_of_rooms);

  add_stairs(cell_array, room_array, *number_of_rooms);

  return 0;
}


