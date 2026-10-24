# Projeto_Ordena-o_De_Vetores

Trabalho **P1 — Programação Paralela (SO2)**.

Aplicação em **C** que consolida e ordena produtos de **5+ lojas fictícias** por
**data**, **valor** e desempate por `id`, usando **Quicksort**, em três versões:

1. **Sequencial**
2. **Paralela com processos + memória compartilhada**
3. **Paralela com threads**

> Detalhes de requisitos, arquitetura e histórico de decisões estão no
> [`SPEC.md`](SPEC.md).

## Estrutura do projeto

```
├── sequencial/            # versão sequencial (código + Makefile + README)
├── processos/             # versão com processos (código + Makefile + README)
├── threads/               # versão com threads (código + Makefile + README)
├── container/             # Dockerfile (Etapa 5)
├── testes/
│   ├── entradas/          # seeds/configurações das entradas
│   └── resultados/        # tempos e speedups medidos
├── documentacao/          # apresentacao.pdf
├── SPEC.md                # especificação viva do projeto
└── README.md              # este arquivo
```

Cada pasta de versão é **autocontida**: contém seu próprio código-fonte, `Makefile`
e `README.md`, conforme a estrutura exigida no enunciado.

## Status

- [x] Etapa 1 — Fundação
- [x] Etapa 2 — Versão sequencial (≥ 2s)
- [x] Etapa 3 — Versão com processos
- [x] Etapa 4 — Versão com threads
- [x] Etapa 5 — Container + medições/speedup

## Pré-requisitos

- **WSL2 + Ubuntu** (ou qualquer Linux com `gcc` e `make`).
- `gcc`, `make` e `libc6-dev`:
  ```bash
  sudo apt update && sudo apt install -y gcc make libc6-dev
  ```

> A versão com processos usa `fork()`/`mmap()` (POSIX), por isso **não** compila no
> Windows nativo — use o WSL2.

## Como compilar e executar

Cada versão tem seu próprio `make` (veja o `README.md` dentro de cada pasta):

```bash
cd sequencial && make && ./sequencial
cd processos  && make && ./processos --proc 4
cd threads    && make && ./threads --threads 4
```

Os valores padrão são 5 lojas × 2.000.000 produtos (~10 milhões de registros),
garantindo execução sequencial ≥ 2 s. As três versões devem produzir o **mesmo
checksum** para a mesma semente.

## Container Docker

A imagem empacota as três versões já compiladas (`container/Dockerfile`):

```bash
# dentro do WSL2
docker build -t ordenacao-produtos -f container/Dockerfile .

# sequencial
docker run --rm ordenacao-produtos ./sequencial/sequencial

# com limite de CPU (1/2/4), como pede o enunciado
docker run --rm --cpus=2 ordenacao-produtos ./processos/processos --proc 4
docker run --rm --cpus=4 ordenacao-produtos ./threads/threads --threads 8
```

## Benchmark e gráficos

Os scripts em `testes/` medem os tempos e geram os gráficos:

```bash
# dentro do WSL2
bash testes/benchmark.sh 5            # nativo (fora do container)
bash testes/benchmark_container.sh    # dentro do container (--cpus=1/2/4)

# no Windows (host), com matplotlib instalado:
python testes\graficos.py
```

Saídas em `testes/resultados/`: `tempos.csv`, `tempos_container.csv` e os gráficos
`grafico1_tempo.png`, `grafico2_speedup.png` e `grafico3_fora_dentro.png`.

> Nota: a máquina de desenvolvimento apresentou alta variância de tempo entre
> execuções; os resultados usam a **mediana** e são aproximados (ver `SPEC.md`).
