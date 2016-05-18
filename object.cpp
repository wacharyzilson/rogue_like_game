#include <iostream>

#include "structures.h"
#include "descriptions.h"
#include "character.h"
#include "npc.h"
#include "dice.h"
#include "dungeon_generator.h"
#include "object.h"
#include "entity_factory.h"

void place_objects(dungeon *d, cell_t *cell_arr, room_t *room_arr,
		   int num_room){
  
  int i, room_i, x, y;
  room_t *room;
  cell_t *cell;

  for(i = 0; i < OBJ_PER_LEVEL; i++)
    {
      while(1)
	{
	  room_i = random_number(0 , num_room - 1);
	
	  room = room_arr + room_i;

	  x = random_number(room->x, room->x + room->width - 1);
	  y = random_number(room->y, room->y + room->height - 1);

	  cell = cell_arr + (MAP_WIDTH * y) + x;

	  if(cell->cell_type == up_stair || cell->cell_type == down_stair)
	    {
	      continue;
	    }
	  
	  if(cell->character == NULL)
	    {
	      break;
	    }
	  if(cell->character->print() != '@')
	    {
	      break;
	    }
	}
      if(cell->object != NULL)
	{
	  cell->object->add_to_stack(d, new object());
	}
      else
	{
	  cell->object = new object();
	  build_object(d, cell->object);
	}
    }
}

void delete_objects(cell_t *cell_arr)
{
  int i, j;
  cell_t *curr;

  for(i = 0; i < MAP_HEIGHT; i++)
    {
      for(j = 0; j < MAP_WIDTH; j++)
	{
	  curr = cell_arr + (MAP_WIDTH * i) + j;
	  
	  if(curr->object != NULL)
	    {
	      delete curr->object;
	    }
	}
    }
}

object::~object()
{
  if(next != NULL)
    {
      delete next;
    }
}

void object::add_to_stack(dungeon_t *d, object *obj)
{
  if(next == NULL)
    {
      next = obj;
      build_object(d, next);
      return;
    }
  
  next->add_to_stack(d, obj);
}

void object::add_to_stack(object *obj)
{
  if(next == NULL)
    {
      next = obj;
      return;
    }
  
  next->add_to_stack(obj);
}

object::object()
{
}

object::object(const object &obj)
{
  name = obj.name;
  symbol = obj.symbol;
  description = obj.description;
  type = obj.type;
  color = obj.color;
  hit = obj.hit;
  dodge = obj.dodge;
  defence = obj.defence;
  weight = obj.weight;
  speed = obj.speed;
  attribute = obj.attribute;
  value = obj.value;
  damage = obj.damage;
  next = obj.next;
}
