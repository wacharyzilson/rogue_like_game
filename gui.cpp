#include <stack>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <ncurses.h>

#include "structures.h"
#include "character.h"
#include "npc.h"
#include "pc.h"
#include "object.h"
#include "inventory.h"
#include "descriptions.h"

//messages to be displayed at the top of the screen
std::stack<std::string> messages;

inventory *inv;
pc *player;
cell_t *map;
int cheat_mode;

const char equip_slots[] = {'a','b','c','d','e','f','g','h','i','j','k','l'};


void add_message(const std::string &message)
{
  messages.push(message);
}

//prints top message on stack w/o removing
static void print_top_message()
{
  if(messages.empty())
    {
      return;
    }

  move(0,0);
  clrtoeol();
  
  mvprintw(0, 0, "(1 of %d) ", messages.size());
  printw("%s", messages.top().c_str());
  refresh();
}

//removes top message then prints next if available
static void remove_print_top_message()
{
  move(0,0);
  clrtoeol();
  refresh();
  
  if(messages.empty())
    {
      return;
    }

  messages.pop();
  print_top_message();
}

void gui_init(inventory *invent, pc *pc_i, cell_t *dungeon)
{
  inv = invent;
  player = pc_i;
  map = dungeon;
  cheat_mode = 0;
  
  initscr();
  keypad(stdscr, TRUE);
  curs_set(0);
  noecho();
  start_color();

  init_pair(COLOR_BLACK, COLOR_WHITE, COLOR_BLACK);
  init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
  init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
  init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
  init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
  init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
  init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
}

void gui_dinit()
{
  endwin();
}

static void print_stats()
{
  move(22,0);
  clrtoeol();
  attron(COLOR_PAIR(COLOR_RED));
  mvprintw(22, 0, "Health: %d", player->hitpoints);
  attroff(COLOR_PAIR(COLOR_RED));

  move(23,0);
  clrtoeol();
  attron(COLOR_PAIR(COLOR_YELLOW));
  mvprintw(23, 0, "Score: %d", player->get_score());
  attroff(COLOR_PAIR(COLOR_YELLOW));
}

void print_map(cell_t *cell_arr)
{
  cell_t *current;
  cell_t *cell_array;

  if(cheat_mode)
    {
      cell_array = map;
    }
  else
    {
      cell_array = cell_arr;
    }
  
  for(int i = 0; i < MAP_HEIGHT; i++)
    {
      for(int j = 0; j < MAP_WIDTH; j++)
	{
	  current = cell_array + (MAP_WIDTH * i) + j;
	  if(current->character != NULL)
	    {
	      if(current->character->print() != '@')
		{
		  attron(COLOR_PAIR(((npc*)current->character)->color));
		  mvaddch(i + 1, j, current->character->print());
		  attroff(COLOR_PAIR(((npc*)current->character)->color));
		}
	      else
		{
		  mvaddch(i + 1, j, current->character->print());
		}
	    }
	  else if(current->object != NULL)
	    {
	      attron(COLOR_PAIR(current->object->get_color()));
	      if(current->object->next != NULL)
		{
		  mvaddch(i + 1, j, '&');
		}
	      else
		{
		  mvaddch(i + 1, j, current->object->print());
		}
	      attroff(COLOR_PAIR(current->object->get_color()));
	    }
	  else if(current->cell_type == stone)
	    {
	      mvaddch(i + 1, j, ' ');
	    }
	  else if(current->cell_type == room)
	    {
	      mvaddch(i + 1, j, '.');
	    }
	  else if(current->cell_type == corridor)
	    {
	      mvaddch(i + 1, j, '#');
	    }
	  else if(current->cell_type == down_stair)
	    {
	      mvaddch(i + 1, j, '>');
	    }
	  else if(current->cell_type == up_stair)
	    {
	      mvaddch(i + 1, j, '<');
	    }
	}
    }
  refresh();

  print_top_message();
  print_stats();
}

//drops item to dungeon floor
static void drop_item()
{
  int input, index;
  cell_t *location;
  std::stringstream mes;
  
  add_message("Which item would you like to drop?");
  print_top_message();

  input = getch();
  switch(input)
    {
    case '0':
      index = 0;
      break;
    case '1':
      index = 1;
      break;
    case '2':
      index = 2;
      break;
    case '3':
      index = 3;
      break;
    case '4':
      index = 4;
      break;
    case '5':
      index = 5;
      break;
    case '6':
      index = 6;
      break;
    case '7':
      index = 7;
      break;
    case '8':
      index = 8;
      break;
    case '9':
      index = 9;
      break;
    case 27:
      add_message("Drop item canceled");
      print_top_message();
      return;
    default:
      add_message("Sorry, that is not a legal slot");
      print_top_message();
      return;
    }

  if(inv->get_carry(index) == NULL)
    {
      add_message("That slot is empty");
      print_top_message();
      return;
    }

  mes << "Removed " << inv->get_carry(index)->get_name();
  add_message(mes.str());
  
  location = map + (MAP_WIDTH * player->y_pos) + player->x_pos;

  inv->remove_from_carry(index, location);

  print_top_message();
}

//displays the description of the item
static void inspect_item()
{
  int input, index, lines, start_y, i, x, y, index_a, index_b;
  std::string desc;
  std::stringstream mes;
  
  add_message("Which item would you like to inspect?");
  print_top_message();

  input = getch();
  switch(input)
    {
    case '0':
      index = 0;
      break;
    case '1':
      index = 1;
      break;
    case '2':
      index = 2;
      break;
    case '3':
      index = 3;
      break;
    case '4':
      index = 4;
      break;
    case '5':
      index = 5;
      break;
    case '6':
      index = 6;
      break;
    case '7':
      index = 7;
      break;
    case '8':
      index = 8;
      break;
    case '9':
      index = 9;
      break;
    case 27:
      add_message("Inspect item canceled");
      print_top_message();
      return;
    default:
      add_message("Sorry, that is not a legal slot");
      print_top_message();
      return;
    }

  print_map(player->sight_arr);

  if(inv->get_carry(index) == NULL)
    {
      add_message("That slot is empty");
      print_top_message();
      return;
    }

  desc = inv->get_carry(index)->get_description();
  
  lines = desc.length() / 40;

  if(desc.length() % 40)
    {
      lines++;
    }

  start_y = 1 + ((21 - (lines + 4)) / 2);

  for(i = 18; i <= 62; i++)
    {
      mvaddch(start_y, i, ' ');
    }

  mvprintw(start_y + 1, 18, " +%s", inv->get_carry(index)->get_name().c_str());

  getyx(stdscr, y, x);
  while(x < 60)
    {
      addch('=');
      getyx(stdscr, y, x);
    }
  printw("+ ");

  index_a = 0;
  index_b = 40;
  for(i = start_y + 2; i < start_y + 2 + lines; i++)
    {
      mvprintw(i, 18, " |");

      while(desc.at(index_a) == ' ')
	{
	  index_a++;
	  index_b++;
	}
      while(index_a < index_b)
	{
	  if(index_a >= desc.length())
	    {
	      addch(' ');
	      index_a++;
	      continue;
	    }
	  
	  if(desc.at(index_a) == '\n')
	    {
	      addch(' ');
	    }
	  else
	    {
	      addch(desc.at(index_a));
	    }
	  index_a++;
	}
      
      printw("| ");
      index_b = index_a + 40;
    }
  
  mvprintw(start_y + lines + 2, 18, " +");
  getyx(stdscr, y, x);
  while(x < 60)
    {
      addch('=');
      getyx(stdscr, y, x);
    }
  printw("+ ");
  
  for(i = 18; i <= 60; i++)
    {
      mvaddch(start_y + lines + 3, i, ' ');
    }

  refresh();

  while(getch() != 27);
}

//removes item from the game entirely
static void expunge_item()
{
  int input, index;
  cell_t *location;
  std::stringstream mes;
  
  add_message("Which item would you like to expunge?");
  print_top_message();

  input = getch();
  switch(input)
    {
    case '0':
      index = 0;
      break;
    case '1':
      index = 1;
      break;
    case '2':
      index = 2;
      break;
    case '3':
      index = 3;
      break;
    case '4':
      index = 4;
      break;
    case '5':
      index = 5;
      break;
    case '6':
      index = 6;
      break;
    case '7':
      index = 7;
      break;
    case '8':
      index = 8;
      break;
    case '9':
      index = 9;
      break;
    case 27:
      add_message("Expunge item canceled");
      print_top_message();
      return;
    default:
      add_message("Sorry, that is not a legal slot");
      print_top_message();
      return;
    }

  if(inv->get_carry(index) == NULL)
    {
      add_message("That slot is empty");
      print_top_message();
      return;
    }

  mes << "Expunged " << inv->get_carry(index)->get_name();
  add_message(mes.str());

  inv->expunge(index);

  print_top_message();
}

//removes item from your equipment
static void dequip_item()
{
  int input, index;
  cell_t *location;
  std::stringstream mes;

  if(!inv->has_space())
    {
      add_message("You do not have enough space to take off an item");
      print_top_message();
      return;
    }
  
  add_message("Which item would you like to take off?");
  print_top_message();

  input = getch();
  switch(input)
    {
    case 'a':
      index = 0;
      break;
    case 'b':
      index = 1;
      break;
    case 'c':
      index = 2;
      break;
    case 'd':
      index = 3;
      break;
    case 'e':
      index = 4;
      break;
    case 'f':
      index = 5;
      break;
    case 'g':
      index = 6;
      break;
    case 'h':
      index = 7;
      break;
    case 'i':
      index = 8;
      break;
    case 'j':
      index = 9;
      break;
    case 'k':
      index = 10;
      break;
    case 'l':
      index = 11;
      break;
    case 27:
      add_message("Take off item canceled");
      print_top_message();
      return;
    default:
      add_message("Sorry, that is not a legal slot");
      print_top_message();
      return;
    }

  if(inv->get_equipment(index) == NULL)
    {
      add_message("You do not have anything equipped in that slot");
      print_top_message();
      return;
    }

  mes << "Took off " << inv->get_equipment(index)->get_name();
  add_message(mes.str());
  
  inv->dequip(index);

  print_top_message();
  player->update_speed();
  print_stats();
}


//adds item to appropriate equipment slot
static void equip_item()
{
  int input, index, index_e;
  cell_t *location;
  std::stringstream mes;
  
  add_message("Which item would you like to equip?");
  print_top_message();

  input = getch();
  switch(input)
    {
    case '0':
      index = 0;
      break;
    case '1':
      index = 1;
      break;
    case '2':
      index = 2;
      break;
    case '3':
      index = 3;
      break;
    case '4':
      index = 4;
      break;
    case '5':
      index = 5;
      break;
    case '6':
      index = 6;
      break;
    case '7':
      index = 7;
      break;
    case '8':
      index = 8;
      break;
    case '9':
      index = 9;
      break;
    case 27:
      add_message("Equip item canceled");
      print_top_message();
      return;
    default:
      add_message("Sorry, that is not a legal slot");
      print_top_message();
      return;
    }

  if(inv->get_carry(index) == NULL)
    {
      add_message("That slot is empty");
      print_top_message();
      return;
    }

  switch(inv->get_carry(index)->get_type())
    {
    case objtype_WEAPON:
      index_e = 0;
      break;
    case objtype_OFFHAND:
      index_e = 1;
      break;
    case objtype_RANGED:
      index_e = 2;
      break;
    case objtype_ARMOR:
      index_e = 3;
      break;
    case objtype_HELMET:
      index_e = 4;
      break;
    case objtype_CLOAK:
      index_e = 5;
      break;
    case objtype_GLOVES:
      index_e = 6;
      break;
    case objtype_BOOTS:
      index_e = 7;
      break;
    case objtype_AMULET:
      index_e = 8;
      break;
    case objtype_LIGHT:
      index_e = 9;
      break;
    case objtype_RING:
      index_e = 10;
      if(inv->get_equipment(index_e) != NULL)
	{
	  index_e = 11;
	}
      break;
    default:
      add_message("That item can not be equipped");
      print_top_message();
      return;
    }

  inv->equip(index, index_e);

  mes << "Equipped " << inv->get_equipment(index_e)->get_name();
  add_message(mes.str());

  print_top_message();
  player->update_speed();
  print_stats();
}




input_t get_keystroke()
{
  int input;
  
  while(1)
    {
      input = getch();

      switch(input)
	{
	case 'y':
	case '7':
	  return up_left;
	case 'k':
	case '8':
	  return up;
	case 'u':
	case '9':
	  return up_right;
	case 'l':
	case '6':
	  return right;
	case 'n':
	case '3':
	  return down_right;
	case 'j':
	case '2':
	  return down;
	case 'b':
	case '1':
	  return down_left;
	case 'h':
	case '4':
	  return left;
	case '<':
	  return u_stair;
	case '>':
	  return d_stair;
	case ' ':
	  return rest;
	case 'S':
	  return save_q;
	case 'm':
	  return m_list;
	case KEY_DOWN:
	  return down_key;
	case KEY_UP:
	  return up_key;
	case 27:
	  return escape;
	case 'z':
	  remove_print_top_message();
	  break;
	case 'i':
	  return invent;
	case 'd':
	  drop_item();
	  return drop;
	case 'e':
	  return equipment;
	case 'w':
	  equip_item();
	  return equip;
	case 't':
	  dequip_item();
	  return dequip;
	case 'x':
	  expunge_item();
	  return expunge;
	case 'I':
	  inspect_item();
	  return inspect;
	case 'c':
	  cheat_mode = !cheat_mode;
	  return c_mode;
	case 'r':
	  return ranged;
	}
    }
}

static int sign(int x)
{
  return (x > 0) - (x < 0);
}



void display_monster_list(character **c_array, int t_char)
{
  character *player, *monster;
  int top, bottom, alive, i, j, x, y;
  input_t input;
  char *x_dir, *y_dir;

  character *m_list[t_char];
  
  const char *north = "north";
  const char *south = "south";
  const char *west = "west";
  const char *east = "east";

  const char *pad_m = "                       ";
  const char *top_m = " +Monsters===========+ ";
  const char *mid_m = " |                   | ";
  const char *bot_m = " +===================+ ";

  alive = 0;

  for(i = 1; i < t_char; i++)
    {
      monster = *(c_array + i);

      if(monster->x_pos < 100)
	{
	  m_list[alive] = monster;
	  alive++;
	}
    }

  top = 0;
  bottom = alive < 13 ? alive : 13;

  player = *(c_array + 0);

  while(1)
    {

      /*place menu border*/

      mvprintw(3, 28, "%s", pad_m);
      mvprintw(4, 28, "%s", top_m);

      for(i = 5; i < 18; i++)
	{
	  mvprintw(i, 28, "%s", mid_m);
	}

      mvprintw(18, 28, "%s", bot_m);
      mvprintw(19, 28, "%s", pad_m);

      j = 5;

      for(i = top; i < bottom; i++)
	{
	  /*print monster stuff here*/
	  
	  monster = m_list[i];

	  x = monster->x_pos - player->x_pos;
	  y = monster->y_pos - player->y_pos;

	  if(sign(x) == 1)
	    {
	      x_dir = (char *) east;
	    }
	  else
	    {
	      x = x * -1;
	      x_dir = (char *) west;
	    }

	  if(sign(y) == 1)
	    {
	      y_dir = (char *) south;
	    }
	  else
	    {
	      y = y * -1;
	      y_dir = (char *) north;
	    }

	  attron(COLOR_PAIR(((npc *)(monster))->color));
	  mvprintw(j, 30, "%c", monster->print());
	  attroff(COLOR_PAIR(((npc *)(monster))->color));
	  mvprintw(j, 31, ": %d %s %d %s", y, y_dir, x, x_dir);
	  j++;
	  
	}

    GET_INPUT : input = get_keystroke();

      switch(input)
	{
	case down_key:
	  if(bottom >= alive)
	    {
	      goto GET_INPUT;
	    }
	  top++;
	  bottom++;
	  break;
	case up_key:
	  if(top <= 0)
	    {
	      goto GET_INPUT;
	    }
	  top--;
	  bottom--;
	  break;
	case escape:
	  goto END;
	default:
	    goto GET_INPUT;
	}
      refresh();
    }
  
 END: clear();
}

void display_inventory()
{
  int i, j, max_string, start_x, end_x, x, y;
  object *current;

  do{
    
    max_string = 7;  

    for(i = 0; i < 10; i ++)
      {
	if(inv->get_carry(i) != NULL)
	  {
	    if(inv->get_carry(i)->get_name().length() > max_string)
	      {
		max_string = inv->get_carry(i)->get_name().length();
	      }
	  }
      }

    print_map(player->sight_arr);
    
    start_x = 40 - (max_string / 2) - 2;
    end_x = start_x + max_string + 7;
    x = 0;
      
    for(i = start_x; i < start_x + max_string + 7; i++)
      {
	mvaddch(4, i, ' ');
      }

    mvaddch(5, start_x, ' ');
    printw("%s", "+Inventory");

    getyx(stdscr, y, x);
    while(x < end_x - 2)
      {
	addch('=');
	getyx(stdscr, y, x);
      }
    printw("%s", "+ ");


    for(i = 0; i < 10; i++)
      {
	mvprintw(6 + i, start_x,  "%s%d%s", " |", i, ". ");
	if(inv->get_carry(i) == NULL)
	  {
	    printw("%s", "(empty)");
	  }
	else
	  {
	    printw("%s", inv->get_carry(i)->get_name().c_str());
	  }
      
	getyx(stdscr, y, x);

	while(x < end_x - 2)
	  {
	    addch(' ');
	    getyx(stdscr, y, x);
	  }
	printw("%s", "| ");
      }

  
    mvprintw(16, start_x, "%s", " +");
    getyx(stdscr, y, x);

    while(x < end_x - 2)
      {
	addch('=');
	getyx(stdscr, y, x);
      }
    printw("%s", "+ ");

  
    for(i = start_x; i < start_x + max_string + 7; i++)
      {
	mvaddch(17, i, ' ');
      }

    refresh();

  }while(get_keystroke() != escape);
}

void display_equipment()
{
  int i, j, max_string, start_x, end_x, x, y;
  object *current;


  do{
    
    max_string = 7;  

    for(i = 0; i < 12; i ++)
      {
	if(inv->get_equipment(i) != NULL)
	  {
	    if(inv->get_equipment(i)->get_name().length() > max_string)
	      {
		max_string = inv->get_equipment(i)->get_name().length();
	      }
	  }
      }

    print_map(player->sight_arr);
    
    start_x = 40 - (max_string / 2) - 2;
    end_x = start_x + max_string + 7;
    x = 0;
      
    for(i = start_x; i < start_x + max_string + 7; i++)
      {
	mvaddch(3, i, ' ');
      }

    mvaddch(4, start_x, ' ');
    printw("%s", "+Equipment");

    getyx(stdscr, y, x);
    while(x < end_x - 2)
      {
	addch('=');
	getyx(stdscr, y, x);
      }
    printw("%s", "+ ");


    for(i = 0; i < 12; i++)
      {
	mvprintw(5 + i, start_x,  "%s%c%s", " |", equip_slots[i], ". ");
	if(inv->get_equipment(i) == NULL)
	  {
	    printw("%s", "(empty)");
	  }
	else
	  {
	    printw("%s", inv->get_equipment(i)->get_name().c_str());
	  }
      
	getyx(stdscr, y, x);

	while(x < end_x - 2)
	  {
	    addch(' ');
	    getyx(stdscr, y, x);
	  }
	printw("%s", "| ");
      }

  
    mvprintw(17, start_x, "%s", " +");
    getyx(stdscr, y, x);

    while(x < end_x - 2)
      {
	addch('=');
	getyx(stdscr, y, x);
      }
    printw("%s", "+ ");

  
    for(i = start_x; i < start_x + max_string + 7; i++)
      {
	mvaddch(18, i, ' ');
      }

    refresh();

  }while(get_keystroke() != escape);
}

int ranged_attack()
{
  int x, y, max_x, min_x, max_y, min_y, select, input, dam;
  cell_t *attack_cell;
  npc *monster;
  object *weapon;
  std::stringstream mes;

  x = player->x_pos;
  y = player->y_pos + 1;

  max_x = x + 3;
  max_y = y + 3;
  min_x = x - 3;
  min_y = y - 3;

  if(min_x < 1)
    {
      min_x = 1;
    }
  else if(max_x > 78)
    {
      max_x = 78;
    }

  if(min_y < 2)
    {
      min_y = 2;
    }
  else if(max_y > 20)
    {
      max_y = 20;
    }

  curs_set(1);
  move(y, x);
  refresh();

  select = 1;

  if((weapon = inv->get_equipment(pc_equip_ranged)) == NULL)
    {
      add_message("You do not have a ranged weapon equipped");
      print_top_message();
      curs_set(0);
      return 0;
    }

  add_message("Where would you like to attack?");
  print_top_message();
  
  while(select)
    {
      if(x < min_x)
	{
	  x = min_x;
	}
      else if(x > max_x)
	{
	  x = max_x;
	}
      
      if(y < min_y)
	{
	  y = min_y;
	}
      else if(y > max_y)
	{
	  y = max_y;
	}

      move(y, x);
      refresh();
	 
      input = getch();
	 
      switch(input)
	{
	case KEY_UP:
	  y--;
	  break;
	case KEY_DOWN:
	  y++;
	  break;
	case KEY_RIGHT:
	  x++;
	  break;
	case KEY_LEFT:
	  x--;
	  break;
	case 'R':
	  select = 0;
	  break;
	case 27:
	  add_message("Attack canceled");
	  print_top_message();
	  curs_set(0);
	  return 0;
	}
    }

  attack_cell = map + (MAP_WIDTH * (y - 1)) + x;

  if(attack_cell->character == NULL)
    {
      add_message("You hit nothing");
      print_top_message();
      curs_set(0);
      return 1;
    }

  if(attack_cell->character->print() == '@')
    {
      add_message("You probably should not attack yourself");
      print_top_message();
      curs_set(0);
      return 0;
    }

  monster = (npc *)attack_cell->character;

  if(!line_of_sight(monster,player, map))
    {
      add_message("You do not have a line of sight to that monster");
      print_top_message();
      curs_set(0);
      return 0;
    }

  dam = monster->attack(weapon->roll_damage());

  if(monster->hitpoints <= 0)
    {
      mes << "You killed " << *monster->name;
      mes << " with an attack of " << dam;
      add_message(mes.str());
      monster->x_pos = 150;
      attack_cell->character = NULL;
    }
  else
    {
      mes << "You hit " << *monster->name;
      mes << " with an attack of " << dam;
      add_message(mes.str());
    }

  

  curs_set(0);
  return 1;
}


void display_win(){

  const char *pad1 = "                              ";
  const char *mes1 = " +==========================+ ";
  const char *mes2 = " |         You Win!         | ";
  const char *pad2 = " |                          | ";
  const char *mes3 = " | Press any key to quit... | ";

  mvprintw(8, 24, "%s", pad1);
  mvprintw(9, 24, "%s", mes1);
  mvprintw(10, 24, "%s", mes2);
  mvprintw(11, 24, "%s", pad2);
  mvprintw(12, 24, "%s", mes3);
  mvprintw(13, 24, "%s", mes1);
  mvprintw(14, 24, "%s", pad1);
  refresh();

  /*wait for input to continue*/

  getch();
}
 
void display_lose(){

  const char *pad1 = "                              ";
  const char *mes1 = " +==========================+ ";
  const char *mes2 = " |         You Lose         | ";
  const char *pad2 = " |                          | ";
  const char *mes3 = " | Press any key to quit... | ";

  mvprintw(8, 24, "%s", pad1);
  mvprintw(9, 24, "%s", mes1);
  mvprintw(10, 24, "%s", mes2);
  mvprintw(11, 24, "%s", pad2);
  mvprintw(12, 24, "%s", mes3);
  mvprintw(13, 24, "%s", mes1);
  mvprintw(14, 24, "%s", pad1);
  refresh();

  /*wait for input to continue*/

  getch();
}



