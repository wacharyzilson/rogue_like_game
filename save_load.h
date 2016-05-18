#ifndef SAVE_LOAD_H
#define SAVE_LOAD_H

int load_number_of_rooms(int *number_of_rooms);
void load(cell_t *cell_array, room_t *room_array, int *number_of_rooms);
void save(cell_t *cell_array, room_t *room_array, int *number_of_rooms);

  
#endif
