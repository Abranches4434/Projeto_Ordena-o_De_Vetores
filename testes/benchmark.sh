#!/usr/bin/env bash
# Benchmark nativo (FORA do container).
#
# Roda cada configuração R vezes, extrai o "tempo total" e grava em CSV:
#   versao,workers,tempo_total
#
# Uso: bash testes/benchmark.sh [rodadas]

set -u

RODADAS="${1:-5}"
LOJAS=5
POR_LOJA=2000000
SEED=42
OUT="testes/resultados/tempos.csv"

RAIZ="$(cd "$(dirname "$0")/.." && pwd)"
cd "$RAIZ"
mkdir -p testes/resultados

# Compila as três versões.
make -C sequencial >/dev/null
make -C processos  >/dev/null
make -C threads    >/dev/null

echo "versao,workers,tempo_total" > "$OUT"

medir() {
  local rotulo="$1" w="$2"; shift 2
  for _ in $(seq 1 "$RODADAS"); do
    local t
    t=$("$@" --lojas "$LOJAS" --por-loja "$POR_LOJA" --seed "$SEED" 2>/dev/null \
        | awk '/tempo total/ {print $4}')
    echo "$rotulo,$w,$t" >> "$OUT"
    sleep 1
  done
}

medir sequencial 1 ./sequencial/sequencial
medir processos  2 ./processos/processos --proc 2
medir processos  4 ./processos/processos --proc 4
medir processos  8 ./processos/processos --proc 8
medir threads    2 ./threads/threads --threads 2
medir threads    4 ./threads/threads --threads 4
medir threads    8 ./threads/threads --threads 8

echo "OK -> $OUT"
