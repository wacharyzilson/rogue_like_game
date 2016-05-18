#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <endian.h>

#include "structures.h"
#include "dungeon_generator.h"

static void initialize_dungeon(cell_t *cell_array)
{
  int i = 0;
  int j = 0;
  cell_t *current;

  for(; i < MAP_HEIGHT; i++)
    {
      for(j = 0; j < MAP_WIDTH; j++)
	{
	  current = cell_array + (MAP_WIDTH * i) + j;
	  current->cell_type = stone;
	  current->part_of_room = NULL;
	  current->character = NULL;
	  current->hardness = 255;
	}
    }
}

static int load_hardness(char *file_path, cell_t *cell_array)
{
  FILE *file;
  int i = 1;
  int j = 1;
  cell_t *current;
  u_int8_t hardness;

  file = fopen(file_path, "r");

  if(file == NULL)
    {
      return -1;
    } 

  fseek(file, 14, SEEK_SET);
  
  for(; i < MAP_HEIGHT - 1; i++)
    {
      for(j = 1; j < MAP_WIDTH - 1; j++)
	{
	  current = cell_array + (MAP_WIDTH * i) + j;
	  fread(&hardness, 1, 1, file);
	  current->hardness = (int) hardness;
	  if(hardness == 0 && current->part_of_room == NULL)
	    {
	      current->cell_type = corridor;
	    }
	}
    }
  fclose(file);
  return 0;
}

static int load_rooms(char *file_path, cell_t *cell_array, room_t *room_array,
	       int number_of_rooms)
{
  FILE *file;
  int i = 0;
  int j = 0;
  int k = 0;
  cell_t *current;
  room_t *current_room;
  u_int8_t buffer;

  file = fopen(file_path, "rb");

  if(file == NULL)
    {
      return -1;
    }

  fseek(file, 1496, SEEK_SET);

  for(; i < number_of_rooms; i++){
    current_room = room_array + i;

    fread(&buffer, 1, 1, file);
    current_room->x = (int) buffer;
    fread(&buffer, 1, 1, file);
    current_room->y = (int) buffer;
    fread(&buffer, 1, 1, file);
    current_room->width = (int) buffer;
    fread(&buffer, 1, 1, file);
    current_room->height = (int) buffer;
    
    for(j = current_room->y; j < current_room->y + current_room->height; j++)
      {
	for(k = current_room->x; k < current_room->x + current_room->width;k++)
	  {
	    current = cell_array + (MAP_WIDTH * j) + k;
	    current->cell_type = room;
	    current->part_of_room = current_room;
	  }
     }
    
  }
  fclose(file);
  return 0;
}

static int write_file(char *file_path, cell_t *cell_array, room_t *room_array,
	       int number_of_rooms)
{
  FILE *file;
  int i = 1;
  int j = 1;
  cell_t *current;
  room_t *current_room;
  int version = 0;
  int file_size;
  u_int8_t buffer;

  file_size = 1496 + (number_of_rooms * 4);
  version = htobe32(version);
  file_size = htobe32(file_size);

  file = fopen(file_path, "wb");

  if(file == NULL)
    {
      return -1;
    }

  fwrite("RLG327", 6, 1, file);
  fwrite(&version, sizeof(int), 1, file);
  fwrite(&file_size, sizeof(int), 1, file);

  for(; i < MAP_HEIGHT - 1; i++)
    {
      for(j = 1; j < MAP_WIDTH - 1; j++)
	{
	  current = cell_array + (MAP_WIDTH * i) + j;
	  buffer = (u_int8_t) current->hardness;
	  fwrite(&buffer, 1, 1, file);
	}
    }

  for(i = 0; i < number_of_rooms; i++)
    {
      current_room = room_array + i;

      buffer = (u_int8_t) current_room->x;
      fwrite(&buffer, 1, 1, file);
      buffer = (u_int8_t) current_room->y;
      fwrite(&buffer, 1, 1, file);
      buffer = (u_int8_t) current_room->width;
      fwrite(&buffer, 1, 1, file);
      buffer = (u_int8_t) current_room->height;
      fwrite(&buffer, 1, 1, file);
    }
  fclose(file);
  return 0;
}

int load_number_of_rooms(int *number_of_rooms)
{
  FILE *file;
  char *file_path;
  int file_path_s;
  
  file_path_s = 16 + strlen(getenv("HOME")) + 1;
  file_path = (char *) malloc(file_path_s);
  snprintf(file_path, file_path_s, getenv("HOME"));
  file_path = strcat(file_path, "/.rlg327/dungeon");
  file = fopen(file_path, "r");

  if(file == NULL)
    {
      free(file_path);
      return -1;
    }

  fseek(file, 10, SEEK_SET);
  fread(number_of_rooms, sizeof(*number_of_rooms), 1, file);
  fclose(file);

  *number_of_rooms = be32toh(*number_of_rooms);
  *number_of_rooms = (*number_of_rooms) - 1496;
  *number_of_rooms = (*number_of_rooms) / 4;

  free(file_path);
  return 0;
}

void load(cell_t *cell_array, room_t *room_array, int *number_of_rooms)
{
  char *file_path;
  int file_path_s;
  
  file_path_s = 16 + strlen(getenv("HOME")) + 1;
  file_path = (char *) malloc(file_path_s);
  snprintf(file_path, file_path_s, getenv("HOME"));
  file_path = strcat(file_path, "/.rlg327/dungeon");

  initialize_dungeon(cell_array);
  load_rooms(file_path, cell_array, room_array, *number_of_rooms);
  load_hardness(file_path, cell_array);

  free(file_path);
}

void save(cell_t *cell_array, room_t *room_array, int *number_of_rooms)
{
  char *file_path;
  int file_path_s;
  
  file_path_s = 16 + strlen(getenv("HOME")) + 1;
  file_path = (char *) malloc(file_path_s);
  snprintf(file_path, file_path_s, getenv("HOME"));
  file_path = strcat(file_path, "/.rlg327/dungeon");
  
  write_file(file_path, cell_array, room_array, *number_of_rooms);  

  free(file_path);
}


