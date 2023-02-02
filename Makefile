CC=gcc
CFLAGS=-std=c2x -Wall -Wextra -Iinclude
LDFLAGS=-lm -pthread
GAME=yun

LDFLAGS+=-Lsubmodules/scieppend -Wl,-rpath=./submodules/scieppend -lscieppend
CFLAGS+=-isystem submodules/scieppend/include/

INCL=$(wildcard include/*.h)
SRCS=$(wildcard src/*.c)
OBJS=$(SRCS:.c=.o)
DEPS=$(SRCS:.c=.d)

.PHONY: clean full-clean debug release profile submodules all all-debug all-clean $(GAME)
.NOTPARALLEL:

$(GAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(GAME) $(LDFLAGS)

%.o: %.c
	$(CC) -MMD $(CFLAGS) -c $< -o $@

submodules:
	make -C submodules/scieppend

all-debug: submodules debug

all: submodules release

release: CFLAGS+=-Ofast
release: $(GAME)

debug: CFLAGS+=-g -DDEBUG
debug: $(GAME)

profile: CFLAGS+=-g -pg
profile: $(GAME)

clean:
	@rm -f $(OBJS)
	@rm -f $(DEPS)

fullclean: clean
	@rm -f $(GAME)

all-clean: fullclean
	make -C submodules/scieppend fullclean

-include $(DEPS)
