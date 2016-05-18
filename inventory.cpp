#include <sstream>
#include <string>
#include <iostream>
#include <stdlib.h>

#include "object.h"
#include "inventory.h"
#include "gui.h"

int object_compare(const void *obj, const void *with)
{
  object **a, **b;
  a = (object **)obj;
  b = (object **)with;

  if((*a) == NULL && (*b) == NULL)
    {
      return 0;
    }
  if((*a) == NULL)
    {
      return 1;
    }
  if((*b) == NULL)
    {
      return -1;
    }

  return (*a)->get_name().compare((*b)->get_name());
}

int inventory::get_speed()
{
  int i, speed;

  speed = 0;

  for(i = 0; i < 12; i++)
    {
      if(equipment[i] != NULL)
	{
	  speed += equipment[i]->get_speed();
	}
    }

  return speed;
}

object *inventory::pick_up_items(object *item_stack)
{
  object *next, *current;
  std::stringstream mes;

  if(!has_space())
    {
      add_message("Inventory is full");
      return item_stack;
    }
  
  current = item_stack;

  while(has_space())
    {
      next = current->next;
      current->next = NULL;
      carry[carrying] = new object(*current);
      carrying++;

      mes << "Picked up " << current->get_name();
      add_message(mes.str());
      mes.str(std::string());

      delete current;
      
      if(next == NULL)
	{
	  sort_carry();
	  return NULL;
	}

      current = next;
    }

  sort_carry();
  return next;
}

void inventory::sort_carry()
{
  qsort(carry, 10, sizeof(object**), object_compare); 
}

void inventory::remove_from_carry(int index, cell_t *loc)
{
  if(loc->object != NULL)
    {
      loc->object->add_to_stack(carry[index]);
    }
  else
    {
      loc->object = carry[index];
    }
  carry[index] = NULL;
  carrying--;
  sort_carry();
}

void inventory::equip(int index_c, int index_e)
{
  object *temp = equipment[index_e];
  equipment[index_e] = carry[index_c];
  carry[index_c] = temp;
  carrying--;
  sort_carry();
}

void inventory::dequip(int index_e)
{
  carry[carrying] = equipment[index_e];
  equipment[index_e] = NULL;
  carrying++;
  sort_carry();
}

void inventory::expunge(int index_c)
{
  delete carry[index_c];
  carry[index_c] = NULL;
  carrying--;
  sort_carry();
}

void inventory::delete_inventory()
{
  int i;

  for(i = 0; i < 10; i++)
    {
      if(carry[i] != NULL)
	{
	  delete carry[i];
	  carry[i] = NULL;
	}
    }

  for(i = 0; i < 12; i ++)
    {
      if(equipment[i] != NULL)
	{
	  delete equipment[i];
	  carry[i] = NULL;
	}
    }
}

int inventory::get_attack_damage()
{
  int damage, i;

  damage = 0;

  for(i = 0; i < 12; i++)
    {
      if(equipment[i] != NULL)
	{
	  damage += equipment[i]->roll_damage();
	}
    }

  return damage;
}

int inventory::get_value()
{
  int value, i;
  value = 0;
  
  for(i = 0; i < 10; i++)
    {
      if(carry[i] != NULL)
	{
	  value += carry[i]->get_value();
	}
    }
  for(i = 0; i < 12; i++)
    {
      if(equipment[i] != NULL)
	{
	  value += equipment[i]->get_value();
	}
    }

  return value;
}

