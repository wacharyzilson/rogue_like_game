#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "heap.h"


#define MAP_HEIGHT 21
#define MAP_WIDTH 80
#define MAX_ROOMS 8
#define MIN_ROOMS 5
#define MAX_ROOM_W 12
#define MIN_ROOM_W 3
#define MAX_ROOM_H 7
#define MIN_ROOM_H 2

#define IS_SMART 0x01
#define IS_TELEPATHIC 0x02
#define IS_TUNNELING 0x04
#define IS_ERATIC 0x08
#define IS_PASS 0x10
#define IS_6 0x20
#define IS_7 0x40
#define IS_8 0x80

#define OBJ_PER_LEVEL 13

typedef enum gamestate{
  player_died,
  update_nt_map,
  player_won,
  normal,
  stairs,
  save_and_quit,
  monster_list,
  dis_inventory,
  drop_it,
  dis_equipment,
  equip_it,
  dequip_it,
  expunge_it,
  inspect_it,
} gamestate_t;

typedef enum input{
  up_left,
  up,
  up_right,
  right,
  down_right,
  down,
  down_left,
  left,
  d_stair,
  u_stair,
  rest,
  save_q,
  m_list,
  down_key,
  up_key,
  escape,
  invent,
  drop,
  equipment,
  equip,
  dequip,
  expunge,
  inspect,
  c_mode,
  ranged,
} input_t;

typedef enum cell_type{
  stone,
  room,
  corridor,
  up_stair,
  down_stair,
} cell_type_t;

typedef struct room{
  int x;
  int y;
  int width;
  int height;
} room_t;

class character;
  
typedef struct cell{
  int hardness;
  room_t *part_of_room;
  cell_type_t cell_type;
  class character *character;
  class object *object;
} cell_t;

typedef struct path_node{
  int x_pos;
  int y_pos;
  int weight;
  int distance;
  heap_node_t *heap_node;
} path_node_t;


  
#endif
