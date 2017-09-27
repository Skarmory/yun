CC=gcc
CFLAGS=-Wall -Iinclude -lncurses

GAME=naxx

INCL=$(wildcard include/*.h)
OBJS=src/colour.o src/race.o src/class.o src/main.o

.PHONY: all clean default

default: $(GAME)

all: $(GAME)

$(GAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(GAME)

src/main.o: src/main.c $(INCL)
	$(CC) $(CFLAGS) -c src/main.c -o src/main.o

src/colour.o: src/colour.c $(INCL)
	$(CC) $(CFLAGS) -c src/colour.c -o src/colour.o

src/class.o: src/class.c $(INCL)
	$(CC) $(CFLAGS) -c src/class.c -o src/class.o

src/race.o: src/race.c $(INCL)
	$(CC) $(CFLAGS) -c src/race.c -o src/race.o

clean:
	@rm -f src/*.o
	@rm -f $(GAME)
