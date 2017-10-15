CC=gcc
CFLAGS=-Wall -Iinclude
LDFLAGS=-lncurses
GAME=naxx

INCL=$(wildcard include/*.h)
OBJS=src/colour.o src/race.o src/class.o src/map.o src/montype.o src/mon.o src/input.o \
     src/util.o src/main.o

.PHONY: all clean default

default: $(GAME)

all: $(GAME)

$(GAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(GAME) $(LDFLAGS)

src/main.o: src/main.c $(INCL)
	$(CC) $(CFLAGS) -c src/main.c -o src/main.o

src/colour.o: src/colour.c $(INCL)
	$(CC) $(CFLAGS) -c src/colour.c -o src/colour.o

src/class.o: src/class.c $(INCL)
	$(CC) $(CFLAGS) -c src/class.c -o src/class.o

src/race.o: src/race.c $(INCL)
	$(CC) $(CFLAGS) -c src/race.c -o src/race.o

src/map.o: src/map.c $(INCL)
	$(CC) $(CFLAGS) -c src/map.c -o src/map.o

src/montype.o: src/montype.c $(INCL)
	$(CC) $(CFLAGS) -c src/montype.c -o src/montype.o

src/mon.o: src/mon.c $(INCL)
	$(CC) $(CFLAGS) -c src/mon.c -o src/mon.o

src/input.o: src/input.c $(INCL)
	$(CC) $(CFLAGS) -c src/input.c -o src/input.o

src/util.o: src/util.c $(INCL)
	$(CC) $(CFLAGS) -c src/util.c -o src/util.o

clean:
	@rm -f src/*.o
	@rm -f $(GAME)
