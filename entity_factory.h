#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H

//populates the character array by selecting a random npc
//definition from definitions.cpp
void build_npcs(dungeon_t *d, character **c_array, int t_char);
//builds the given object by selecting a random object definition
//from definitions.cpp
void build_object(dungeon_t *d, object *obj);

#endif
