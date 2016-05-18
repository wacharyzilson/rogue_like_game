#ifndef DUNGEON_GENERATOR_H
#define DUNGEON_GENERATOR_H

//generates random number between min and max, inclusive
int random_number(int min, int max);

//generates a new dungeon
int generate_dungeon(cell_t *cell_array, room_t *room_array,
		     int *number_of_rooms);


#endif
