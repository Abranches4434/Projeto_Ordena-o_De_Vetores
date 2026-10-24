# Dockerfile — Ordenação de Produtos Multi-loja (Trabalho P1 — SO2)
#
# Constrói uma imagem com as três versões compiladas.
#
# Uso:
#   docker build -t ordenacao-produtos -f container/Dockerfile .
#
#   # versão sequencial
#   docker run --rm ordenacao-produtos ./sequencial/sequencial
#
#   # versão com processos (2 processos, limitado a 1 CPU)
#   docker run --rm --cpus=1 ordenacao-produtos ./processos/processos --proc 2
#
#   # versão com threads (4 threads, limitado a 2 CPUs)
#   docker run --rm --cpus=2 ordenacao-produtos ./threads/threads --threads 4

FROM ubuntu:24.04

# Dependências de compilação (gcc, make e cabeçalhos da libc).
RUN apt-get update && apt-get install -y --no-install-recommends \
        gcc make libc6-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Código das três versões (cada pasta é autocontida).
COPY sequencial/ ./sequencial/
COPY processos/ ./processos/
COPY threads/ ./threads/

# Compila as três versões.
RUN make -C sequencial \
 && make -C processos \
 && make -C threads

# Comando padrão: versão sequencial.
CMD ["./sequencial/sequencial"]
