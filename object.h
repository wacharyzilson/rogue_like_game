#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include "structures.h"
#include "descriptions.h"
#include "dice.h"

void place_objects(dungeon_t *d, cell_t *cell_arr, room_t *room_arr,
		   int num_room);
void delete_objects(cell_t *cell_arr);

class object{
  friend class object_description;
private:
  std::string *name, *description;
  char symbol;
  object_type_t type;
  int color, hit, dodge, defence, weight, speed, attribute, value;
  dice *damage;
public:
  object *next;
  object();
  ~object();
  object(const object &obj);
  inline char print(){return symbol; };
  inline int get_color(){return color; };
  inline std::string &get_name(){return *name; };
  inline object_type_t get_type(){return type; };
  inline std::string &get_description(){return *description; };
  inline int get_speed(){return speed; };
  inline int roll_damage(){return damage->roll(); };
  inline int get_value(){return value; };
  void add_to_stack(dungeon *d, object *obj);
  void add_to_stack(object *obj);
};

#endif
