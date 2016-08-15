CC?=gcc
CFLAGS+=-Wall -g3 -Iinclude/
LDFLAGS+=
GAME_OBJS=src/allocator.o src/oop.o src/game.o src/world.o src/mob.o src/player.o src/item.o

all: game roomtool mobtool

game: $(GAME_OBJS)
	$(CC) $(LDFLAGS) $(GAME_OBJS) -o game

roomtool: src/roomtool.o
	$(CC) $(LDFLAGS) $< -o $@

mobtool: src/mobtool.o
	$(CC) $(LDFLAGS) $< -o $@

*.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o game mobtool roomtool 

superclean: clean
	rm -f include/*~ src/*~
