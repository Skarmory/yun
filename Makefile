CC=gcc
CFLAGS=-std=c2x -Wall -Wextra -Iinclude
LDFLAGS=-lm -pthread
GAME=yun

LDFLAGS+=-Lsubmodules/scieppend -Wl,-rpath=./submodules/scieppend -lscieppend
CFLAGS+=-isystem submodules/scieppend/include/

INCL=$(wildcard include/*.h)
INCL_SYS=$(wildcard include/systems/*.h)
INCL_COM=$(wildcard include/components/*.h)
INCL_MAPGEN=$(wildcard include/map_gen/*.h)
SRCS=$(wildcard src/*.c)
SRCS_SYS=$(wildcard src/systems/*.c)
SRCS_COM=$(wildcard src/components/*.c)
SRCS_MAP_GEN=$(wildcard src/map_gen/*.c)
OBJS=$(SRCS:.c=.o)
OBJS_SYS=$(SRCS_SYS:.c=.o)
OBJS_COM=$(SRCS_COM:.c=.o)
OBJS_MAP_GEN=$(SRCS_MAP_GEN:.c=.o)
DEPS=$(SRCS:.c=.d)
DEPS_SYS=$(SRCS_SYS:.c=.d)
DEPS_COM=$(SRCS_COM:.c=.d)
DEPS_MAP_GEN=$(SRCS_MAP_GEN:.c=.d)

.PHONY: clean full-clean debug release profile submodules all all-debug all-clean $(GAME)
.NOTPARALLEL:

$(GAME): $(OBJS) $(OBJS_SYS) $(OBJS_COM) $(OBJS_MAP_GEN)
	$(CC) $(CFLAGS) $(OBJS) $(OBJS_SYS) $(OBJS_COM) $(OBJS_MAP_GEN) -o $(GAME) $(LDFLAGS)

%.o: %.c
	$(CC) -MMD $(CFLAGS) -c $< -o $@

submodules:
	make -C submodules/scieppend

submodules-debug:
	make -C submodules/scieppend debug

all-debug: submodules-debug debug

all: submodules release

release: CFLAGS+=-Ofast
release: $(GAME)

debug: CFLAGS+=-g -DDEBUG
debug: $(GAME)

profile: CFLAGS+=-g -pg
profile: $(GAME)

clean:
	@rm -f $(OBJS)
	@rm -f $(OBJS_SYS)
	@rm -f $(OBJS_COM)
	@rm -f $(OBJS_MAP_GEN)
	@rm -f $(DEPS)
	@rm -f $(DEPS_SYS)
	@rm -f $(DEPS_COM)
	@rm -f $(DEPS_MAP_GEN)

fullclean: clean
	@rm -f $(GAME)

all-clean: fullclean
	make -C submodules/scieppend fullclean

-include $(DEPS)
-include $(DEPS_SYS)
-include $(DEPS_COM)
-include $(DEPS_MAP_GEN)
