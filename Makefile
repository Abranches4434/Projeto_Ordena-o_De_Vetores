# Makefile — Ordenação de Produtos Multi-loja (Trabalho P1 — SO2)
#
# Compilação e execução são feitas no ambiente WSL2/Ubuntu (gcc + make).

CC      ?= gcc
CFLAGS  ?= -std=gnu11 -O2 -Wall -Wextra
LDFLAGS ?=

BIN_DIR := bin
SRC_DIR := src

# Fontes comuns a todas as versões.
COMUM_SRC := $(SRC_DIR)/gerador.c $(SRC_DIR)/quicksort.c $(SRC_DIR)/util.c

SEQ_SRC  := $(SRC_DIR)/main_sequencial.c $(COMUM_SRC)
PROC_SRC := $(SRC_DIR)/main_processos.c $(COMUM_SRC)
THR_SRC  :=

SEQ_BIN  := $(BIN_DIR)/sequencial
PROC_BIN := $(BIN_DIR)/processos

.PHONY: all sequencial processos threads clean help

all: sequencial processos

sequencial: $(SEQ_BIN)

$(SEQ_BIN): $(SEQ_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(SEQ_SRC) $(LDFLAGS)

processos: $(PROC_BIN)

$(PROC_BIN): $(PROC_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(PROC_SRC) $(LDFLAGS)

threads:
	@echo "Versao com threads ainda nao implementada (Etapa 4)."

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BIN_DIR)

help:
	@echo "Alvos disponiveis:"
	@echo "  make sequencial  - versao sequencial (Etapa 2)"
	@echo "  make processos   - versao com processos + memoria compartilhada (Etapa 3)"
	@echo "  make threads     - (futuro) versao com threads (Etapa 4)"
	@echo "  make clean       - remove artefatos de build"
