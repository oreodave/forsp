CC=gcc
OUT=main.out
LIBS=
ARGS=

RELEASE=0
GFLAGS=-Wall -Wextra -Werror -Wswitch-enum -std=c11 -I./include
DFLAGS=-ggdb -fsanitize=address -fsanitize=undefined
RFLAGS=-O3
ifeq ($(RELEASE), 1)
CFLAGS=$(GFLAGS) $(RFLAGS)
else
CFLAGS=$(GFLAGS) $(DFLAGS)
endif

.PHONY: all
all: $(OUT)

$(OUT): src/vec.c src/obj.c src/parser.c src/main.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

.PHONY: run
run: $(OUT)
	./$^ $(ARGS)

.PHONY:
clean:
	git clean -fdx
