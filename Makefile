# Makefile — Ordenação de Produtos Multi-loja (Trabalho P1 — SO2)
#
# Etapa 1 (fundação): apenas define o ambiente de build.
# Os alvos `sequencial`, `processos` e `threads` serão adicionados nas
# Etapas 2, 3 e 4, respectivamente.

CC      ?= gcc
CFLAGS  ?= -std=c11 -O2 -Wall -Wextra
LDFLAGS ?=

BIN_DIR := bin

.PHONY: all clean help

all: help

help:
	@echo "Etapa 1 (fundacao): nenhum alvo de compilacao ainda."
	@echo "Alvos previstos: make sequencial | make processos | make threads"

clean:
	@echo "Nada a limpar na Etapa 1."
