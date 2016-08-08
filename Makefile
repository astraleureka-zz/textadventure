CC?=gcc
CFLAGS+=-Wall -g3
LDFLAGS+=

all: game roomtool mobtool

game: oop.o game.o
	$(CC) $(LDFLAGS) oop.o game.o -o game

roomtool: roomtool.o
	$(CC) $(LDFLAGS) roomtool.o -o roomtool

mobtool: mobtool.o
	$(CC) $(LDFLAGS) mobtool.o -o mobtool

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o game mobtool romtool 
