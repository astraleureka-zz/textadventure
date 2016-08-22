APP_NAME=game
CXREF?=cxref
DOCS?=1
INCLUDES+=-Iinclude/ 
DEFINES+=
CFLAGS+=-D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wno-unused-parameter -pedantic -std=c99 -g3 $(INCLUDES) $(DEFINES)
LDFLAGS+=
GAME_OBJS=src/allocator.o src/oop.o src/game.o src/world.o src/mob.o src/player.o src/item.o src/combat.o src/util.o

all: game roomtool mobtool

game: $(GAME_OBJS)
	$(CC) $(LDFLAGS) $(GAME_OBJS) -o game

roomtool: src/roomtool.o
	$(CC) $(LDFLAGS) $< -o $@

mobtool: src/mobtool.o
	$(CC) $(LDFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
ifeq ($(DOCS), 1)
	$(CXREF) -Odocs -N$(APP_NAME) -html-src $(INCLUDES) $(DEFINES) $<
endif

clean:
	rm -f src/*.o game mobtool roomtool 

superclean: clean
	rm -f include/*~ src/*~
