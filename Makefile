# Makefile — Ordenação de Produtos Multi-loja (Trabalho P1 — SO2)
#
# Compilação e execução são feitas no ambiente WSL2/Ubuntu (gcc + make).

CC      ?= gcc
CFLAGS  ?= -std=gnu11 -O2 -Wall -Wextra
LDFLAGS ?=

BIN_DIR := bin
SRC_DIR := src

# Fontes da versão sequencial (Etapa 2).
SEQ_SRC := $(SRC_DIR)/main_sequencial.c $(SRC_DIR)/gerador.c $(SRC_DIR)/quicksort.c
SEQ_BIN := $(BIN_DIR)/sequencial

# Fontes das versões paralelas (Etapas 3 e 4) — preenchidos nas próximas etapas.
PROC_SRC :=
THR_SRC  :=

.PHONY: all sequencial processos threads clean help

all: sequencial

sequencial: $(SEQ_BIN)

$(SEQ_BIN): $(SEQ_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(SEQ_SRC) $(LDFLAGS)

# Alvos futuros (Etapas 3 e 4):
processos:
	@echo "Versao com processos ainda nao implementada (Etapa 3)."

threads:
	@echo "Versao com threads ainda nao implementada (Etapa 4)."

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BIN_DIR)

help:
	@echo "Alvos disponiveis:"
	@echo "  make sequencial  - compila a versao sequencial (Etapa 2)"
	@echo "  make processos   - (futuro) versao com processos"
	@echo "  make threads     - (futuro) versao com threads"
	@echo "  make clean       - remove artefatos de build"
