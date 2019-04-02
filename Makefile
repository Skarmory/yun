CC=gcc
CFLAGS=-Wall -Iinclude
LDFLAGS=-lm
GAME=naxx

INCL=$(wildcard include/*.h)
SRCS=$(wildcard src/*.c)
OBJS=$(SRCS:.c=.o)
DEPS=$(SRCS:.c=.d)

.PHONY: clean fullclean $(GAME)

$(GAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(GAME) $(LDFLAGS)

%.o: %.c
	$(CC) -MMD $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJS)
	@rm -f $(DEPS)

fullclean: clean
	@rm -f $(GAME)

-include $(DEPS)
