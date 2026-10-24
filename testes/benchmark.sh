#!/usr/bin/env bash
# Benchmark nativo (FORA do container), com execuções INTERCALADAS (round-robin).
#
# Em vez de rodar todas as execuções de uma configuração seguidas, cada "rodada"
# executa UMA vez cada configuração. Isso faz com que todas sejam medidas sob as
# MESMAS condições da máquina (frequência/temperatura), tornando as medianas
# comparáveis entre si.
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

medir_um() {
  local rotulo="$1" w="$2"; shift 2
  local t
  t=$("$@" --lojas "$LOJAS" --por-loja "$POR_LOJA" --seed "$SEED" 2>/dev/null \
      | awk '/tempo total/ {print $4}')
  echo "$rotulo,$w,$t" >> "$OUT"
}

# Uma rodada = uma execução de cada configuração, em ordem fixa.
for _ in $(seq 1 "$RODADAS"); do
  medir_um sequencial 1 ./sequencial/sequencial
  medir_um processos  2 ./processos/processos --proc 2
  medir_um processos  4 ./processos/processos --proc 4
  medir_um processos  8 ./processos/processos --proc 8
  medir_um threads    2 ./threads/threads --threads 2
  medir_um threads    4 ./threads/threads --threads 4
  medir_um threads    8 ./threads/threads --threads 8
done

echo "OK -> $OUT"
