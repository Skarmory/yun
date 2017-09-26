CC=gcc
CFLAGS=-Wall -Iinclude -lncurses

GAME=naxx

.PHONY: all clean default

default: $(GAME)

all: $(GAME)

$(GAME): src/main.o
	$(CC) $(CFLAGS) src/main.o -o $(GAME)

main.o: src/main.c
	$(CC) $(CFLAGS) -c src/main.c

clean:
	@rm -f src/*.o
	@rm -f $(GAME)
