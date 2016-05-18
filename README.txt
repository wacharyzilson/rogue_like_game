Author: Zachary Wilson

Main Executable: main 

This program accepts --nummon=n (where n is number of monsters in a dungeon,
default is 10) argument.

Compile with make

monster_desc.txt and object_desc.txt need to be moved to the directory:
HOME/.rlg327/
where HOME is your home directory

Controls

7 or y - move up and left
8 or k - move up
9 or u - move up and right
6 or l - move right
3 or n - move down right
2 or j - move down
l or b - move down left
4 or h - move left
> - go down stairs
< go up stairs
(going up or down stairs generates a new dungeon)
space - rest (npcs still move)
m - lists all monsters in game
  --scroll through with arrow keys
esc - exits a menu or command
S - quit
w - wear an item
t - take off an item
d - drop and item to the floor
x - remove an item in your inventory from the game
i - list your inventory
e - list your equipment
I - inspect an item
z - next message in message queue
r-enter ranged combat mode (only if a RANGED item is equipped)
	-use cursor to select target with in a radius of 3
	 of the player (same as sight of player
	-attack only rolls the damage dice of the RANGED item
R-attacks the target under the cursor in ranged combat mode








