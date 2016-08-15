CC?=gcc
CFLAGS+=-Wall -g3
LDFLAGS+=
GAME_OBJS=allocator.o oop.o game.o world.o mob.o player.o 

all: game roomtool mobtool

game: $(GAME_OBJS)
	$(CC) $(LDFLAGS) $(GAME_OBJS) -o game

roomtool: roomtool.o
	$(CC) $(LDFLAGS) roomtool.o -o roomtool

mobtool: mobtool.o
	$(CC) $(LDFLAGS) mobtool.o -o mobtool

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o game mobtool romtool 

superclean: clean
	rm -f *~
