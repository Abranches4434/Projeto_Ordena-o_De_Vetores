#!/usr/bin/env bash
# Benchmark DENTRO do container, com limites de CPU (--cpus=1/2/4).
#
# Constrói a imagem e roda as versões paralelas (2/4/8 workers) em containers
# com 1, 2 e 4 CPUs. Grava em CSV:
#   versao,workers,cpus,tempo_total
#
# Uso: bash testes/benchmark_container.sh

set -u

LOJAS=5
POR_LOJA=2000000
SEED=42
OUT="testes/resultados/tempos_container.csv"
IMAGEM="ordenacao-produtos"

RAIZ="$(cd "$(dirname "$0")/.." && pwd)"
cd "$RAIZ"
mkdir -p testes/resultados

# Constrói a imagem (contexto = raiz do projeto, Dockerfile em container/).
docker build -t "$IMAGEM" -f container/Dockerfile . >/dev/null

echo "versao,workers,cpus,tempo_total" > "$OUT"

for cpus in 1 2 4; do
  for p in 2 4 8; do
    t=$(docker run --rm --cpus="$cpus" "$IMAGEM" \
        ./processos/processos --proc "$p" --lojas "$LOJAS" --por-loja "$POR_LOJA" --seed "$SEED" 2>/dev/null \
        | awk '/tempo total/ {print $4}')
    echo "processos,$p,$cpus,$t" >> "$OUT"
  done
  for th in 2 4 8; do
    t=$(docker run --rm --cpus="$cpus" "$IMAGEM" \
        ./threads/threads --threads "$th" --lojas "$LOJAS" --por-loja "$POR_LOJA" --seed "$SEED" 2>/dev/null \
        | awk '/tempo total/ {print $4}')
    echo "threads,$th,$cpus,$t" >> "$OUT"
  done
done

echo "OK -> $OUT"
