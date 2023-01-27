CC=gcc
CFLAGS=-Wall -Wextra -Iinclude
LDFLAGS=-lm -pthread
GAME=yun

INCL=$(wildcard include/*.h)
SRCS=$(wildcard src/*.c)
OBJS=$(SRCS:.c=.o)
DEPS=$(SRCS:.c=.d)

.PHONY: clean fullclean debug fast $(GAME)

$(GAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(GAME) $(LDFLAGS)

%.o: %.c
	$(CC) -MMD $(CFLAGS) -c $< -o $@

fast: CFLAGS += -Ofast
fast: $(GAME)

debug: CFLAGS += -DDEBUG -g
debug: $(GAME)

profile: CFLAGS += -g -pg
profile: $(GAME)

clean:
	@rm -f $(OBJS)
	@rm -f $(DEPS)

fullclean: clean
	@rm -f $(GAME)

-include $(DEPS)
