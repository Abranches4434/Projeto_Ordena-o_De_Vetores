CC     ?= gcc
CFLAGS ?= -std=gnu11 -O2 -Wall -Wextra

SRC := main_processos.c gerador.c quicksort.c util.c
HDR := modelo.h gerador.h quicksort.h util.h
BIN := processos

.PHONY: all clean

all: $(BIN)

$(BIN): $(SRC) $(HDR)
	$(CC) $(CFLAGS) -o $@ $(SRC)

clean:
	rm -f $(BIN)
