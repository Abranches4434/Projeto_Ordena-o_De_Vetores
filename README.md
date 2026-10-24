# Projeto_Ordena-o_De_Vetores

Trabalho **P1 — Programação Paralela (SO2)**.

Aplicação em **C** que consolida e ordena produtos de **5+ lojas fictícias** por
**data**, **valor** e desempate por `id`, usando **Quicksort**, em três versões:

1. **Sequencial** ✅
2. **Paralela com processos + memória compartilhada**
3. **Paralela com threads**

> Detalhes de requisitos, arquitetura e histórico de decisões estão no
> [`SPEC.md`](SPEC.md).

## Estrutura do projeto

```
├── SPEC.md                 # especificação viva do projeto
├── README.md               # este arquivo
├── Makefile                # build das três versões
├── src/
│   ├── modelo.h            # struct Produto + comparador
│   ├── gerador.c/.h        # geração sintética por loja
│   ├── quicksort.c/.h      # quicksort sequencial/paralelo
│   ├── main_sequencial.c   # versão sequencial
│   ├── main_processos.c    # versão com processos (Etapa 3)
│   └── main_threads.c      # versão com threads (Etapa 4)
├── testes/
│   ├── entradas/           # seeds/configurações
│   └── resultados/         # tempos e speedups
└── container/
    └── Dockerfile          # container (Etapa 5)
```

## Status

- [x] Etapa 1 — Fundação (modelo de dados + estrutura + SPEC)
- [x] Etapa 2 — Versão sequencial (≥ 2s)
- [ ] Etapa 3 — Versão com processos
- [ ] Etapa 4 — Versão com threads
- [ ] Etapa 5 — Container + medições/speedup

## Pré-requisitos

- **WSL2 + Ubuntu** (ou qualquer Linux com `gcc` e `make`).
- `gcc` e `make` instalados:
  ```bash
  sudo apt update && sudo apt install -y gcc make
  ```

> A versão com processos usa `fork()`/`mmap()` (POSIX), por isso **não** compila
> no Windows nativo — use o WSL2.

## Como compilar e executar

Dentro do WSL2, acesse o projeto (a partir do Windows):

```bash
cd /mnt/c/Users/jo22m/Projeto_Ordena-o_De_Vetores
```

Compile a versão sequencial:

```bash
make sequencial
```

Execute (valores padrão: 5 lojas × 2.000.000 produtos):

```bash
./bin/sequencial
```

Parâmetros opcionais:

```bash
./bin/sequencial --lojas 8 --por-loja 2000000 --seed 42 --saida testes/resultados/saida.bin
```

| Parâmetro | Descrição |
|-----------|-----------|
| `--lojas N` | quantidade de lojas (padrão 5) |
| `--por-loja M` | produtos por loja (padrão 2.000.000) |
| `--seed S` | semente do gerador (padrão 0) |
| `--saida ARQ` | grava o vetor ordenado em arquivo binário |
| `--help` | mostra a ajuda |

### Exemplo de saída

```
Resultados (sequencial):
  total de produtos : 10000000
  ordenacao verificada: OK
  checksum          : 730a917b356d1467
  tempo geracao     : 1.883 s
  tempo ordenacao   : 5.375 s
  tempo saida       : 0.091 s
  tempo total       : 7.349 s
  primeiros 3: ...
  ultimos 3: ...
```

O `checksum` é usado para verificar que as três versões produzem exatamente a
mesma saída para a mesma semente.
