#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "structures.h"
#include "heap.h"
#include "dungeon_generator.h"
#include "path_finder.h"
#include "character.h"

int path_node_cmp(const void *key, const void *with)
{
  path_node_t *t_key = (path_node_t *) key;
  path_node_t *t_with = (path_node_t *) with;

  return t_key->distance - t_with->distance;
}

static void init_tunn_heap(heap_t *h, path_node_t *path_arr, cell_t *cell_arr)
{
  int i, j;
  path_node_t *curr_p;
  cell_t *curr_c;

  for(i = 0; i < MAP_HEIGHT; i++)
    {
      for(j = 0; j < MAP_WIDTH; j++)
	{
	  curr_c = cell_arr + (MAP_WIDTH * i) + j;
	  curr_p = path_arr + (MAP_WIDTH * i) + j;

	  curr_p->x_pos = j;
	  curr_p->y_pos = i;
	  curr_p->distance = INT_MAX;
	  
	  if(curr_c->hardness ==  255)
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
	      curr_p->weight = 1;
	    }
	  else if(curr_c->hardness < 171)
	    {
	      curr_p->weight = 2;
	    }
	  else
	    {
	      curr_p->weight = 3;
	    }

	  if(curr_c->character != NULL && curr_c->character->print() == '@')
	    {
	      curr_p->distance = 0;
	    }
	  
	  curr_p->heap_node = heap_insert(h, curr_p);
	}
    }
}

static void init_non_tunn_heap(heap_t *h,
			       path_node_t *path_arr, cell_t *cell_arr)
{
  int i, j;
  path_node_t *curr_p;
  cell_t *curr_c;

  for(i = 0; i < MAP_HEIGHT; i++)
    {
      for(j = 0; j < MAP_WIDTH; j++)
	{
	  curr_c = cell_arr + (MAP_WIDTH * i) + j;
	  curr_p = path_arr + (MAP_WIDTH * i) + j;

	  curr_p->x_pos = j;
	  curr_p->y_pos = i;
	  curr_p->distance = INT_MAX;

	  if(curr_c->character != NULL
	     && curr_c->character->print() == '@')
	    {
	      curr_p->weight = 1;
	      curr_p->distance = 0;
	      curr_p->heap_node = heap_insert(h, curr_p);
	    }
	  else if(curr_c->hardness ==  0)
	    {
	      curr_p->weight = 1;
	      curr_p->heap_node = heap_insert(h, curr_p);
	    }
	  else
	    {
	      curr_p->heap_node = NULL;
	    }
	}
    }
}

static void find_paths(heap_t *h, path_node_t *path_arr)
{
  int i, j;
  path_node_t  *curr_p, *curr_n_p;
   
  while(h->size > 0)
    {
      curr_p = (path_node_t *) heap_remove_min(h);
      curr_p->heap_node = NULL;
      
      for(i = curr_p->y_pos - 1; i < curr_p->y_pos + 2; i++)
	{
	  for(j = curr_p->x_pos - 1; j < curr_p->x_pos + 2; j++)
	    {
	      curr_n_p = path_arr + (MAP_WIDTH * i) + j;

	      if(curr_n_p->heap_node == NULL)
		{
		  continue;
		}


	      if(curr_n_p->distance > curr_p->distance + curr_n_p->weight)
		{
		  curr_n_p->distance = curr_p->distance + curr_n_p->weight;
		  heap_decrease_key_no_replace(h, curr_n_p->heap_node);
		}
	    }
	}
    }
}

void print_path_map(path_node_t *path_arr)
{
  path_node_t *curr_p;
  int i, j;
  char converter[] =
    "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

  for(i = 0; i < MAP_HEIGHT; i++)
    {
      for(j = 0; j < MAP_WIDTH; j++)
	{
	  curr_p = path_arr + (MAP_WIDTH * i) + j;
	  if(curr_p->distance <= 61)
	    {
	      printf("%c", converter[curr_p->distance]);
	    }
	  else
	    {
	      printf(" ");
	    }
	}
      printf("\n");
    }
}

void update_tunn_map(path_node_t *path_arr, cell_t *cell_arr)
{
  heap_t *h;

  h = (heap *)malloc(sizeof(*h));

  heap_init(h, path_node_cmp, NULL);
  init_tunn_heap(h, path_arr, cell_arr);
  find_paths(h, path_arr);
  heap_delete(h);
  
  free(h);
}

void update_non_tunn_map(path_node_t *path_arr, cell_t *cell_arr)
{
  heap_t *h;

  h = (heap *)malloc(sizeof(*h));

  heap_init(h, path_node_cmp, NULL);
  init_non_tunn_heap(h, path_arr, cell_arr);
  find_paths(h, path_arr);
  heap_delete(h);
  
  free(h);
}




