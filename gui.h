#ifndef GUI_H
#define GUI_H

#include <string>
#include "inventory.h"
#include "character.h"
#include "pc.h"

//adds message to the message queue at the top of the screen
void add_message(const std::string &message);
//initilizes gui
void gui_init(inventory *invent, pc *pc_i, cell_t *dungeon);
//ends gui
void gui_dinit();
//outputs map to ncurses terminal
void print_map(cell_t *cell_arr);
//returns a keystroke when a valid key in the game is pressed
input_t get_keystroke();
//displaces list of all monsters in current dungeon
void display_monster_list(character **c_array, int t_char);
//displays pc inventory
void display_inventory();
//displays pc equipment
void display_equipment();
//displays win message
void display_win();
//displays lose message
void display_lose();
//method for performing a ranged attack on an enemy, returns damage delt
int ranged_attack();


#endif
