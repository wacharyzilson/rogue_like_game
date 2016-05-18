LIB = -lncurses -lm
CFLAGS = -g 
OBJ = main.o dungeon_generator.o gui.o heap.o path_finder.o save_load.o npc.o pc.o character.o descriptions.o dice.o entity_factory.o object.o inventory.o

all: main

main: $(OBJ)
	g++ -o main $(OBJ) $(LIB) $(CFLAGS)

main.o: main.cpp
	g++ -c $(CFLAGS) main.cpp

dungeon_generator.o: dungeon_generator.cpp
	g++ -c $(CFLAGS) dungeon_generator.cpp

gui.o: gui.cpp
	g++ -c $(CFLAGS) gui.cpp

heap.o: heap.c
	gcc -c $(CFLAGS) heap.c

path_finder.o: path_finder.cpp
	g++ -c $(CFLAGS) path_finder.cpp

save_load.o: save_load.cpp
	g++ -c $(CFLAGS) save_load.cpp

npc.o: npc.cpp
	g++ -c $(CFLAGS) npc.cpp

pc.o: pc.cpp
	g++ -c $(CFLAGS) pc.cpp

character.o: character.cpp
	g++ -c $(CFLAGS) character.cpp

descriptions.o: descriptions.cpp
	g++ -c $(CFLAGS) descriptions.cpp

dice.o: dice.cpp
	g++ -c $(CFLAGS) dice.cpp

entity_factory.o: entity_factory.cpp
	g++ -c $(CFLAGS) entity_factory.cpp

object.o: object.cpp
	g++ -c $(CFLAGS) object.cpp

inventory.o: inventory.cpp
	g++ -c $(CFLAGS) inventory.cpp

clean:
	rm -f main
	rm -f $(OBJ)
	rm -f *~ \#*
	rm -f log.txt







