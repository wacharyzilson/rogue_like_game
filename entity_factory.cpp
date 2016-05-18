#include <vector>
#include <iostream>

#include "structures.h"
#include "descriptions.h"
#include "character.h"
#include "npc.h"
#include "dice.h"
#include "dungeon_generator.h"
#include "object.h"


void build_npcs(dungeon_t *d, character **c_array, int t_char)
{
  int i, rand;
  npc *monster;

  for(i = 1; i < t_char; i++)
    {
      monster  = (npc*)(*(c_array + i));
      
      rand = random_number(0, d->monster_descriptions.size() - 1);
      d->monster_descriptions[rand].build(monster);
						
    }
}

void build_object(dungeon_t *d, object *obj)
{
  int rand;
 
  rand = random_number(0, d->object_descriptions.size() - 1);
  d->object_descriptions[rand].build(obj);

}
