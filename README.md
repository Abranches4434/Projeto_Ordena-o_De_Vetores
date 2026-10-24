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
├── SPEC.md                 # especificação viva do projeto
├── README.md               # este arquivo
├── Makefile                # build das três versões
├── src/
│   ├── modelo.h            # struct Produto + comparador
│   ├── gerador.c/.h        # geração sintética por loja (Etapa 2)
│   ├── quicksort.c/.h      # quicksort sequencial/paralelo (Etapas 2–4)
│   ├── main_sequencial.c   # versão sequencial (Etapa 2)
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
- [ ] Etapa 2 — Versão sequencial (≥ 2s)
- [ ] Etapa 3 — Versão com processos
- [ ] Etapa 4 — Versão com threads
- [ ] Etapa 5 — Container + medições/speedup

## Como compilar e executar

> Instruções serão preenchidas a partir da **Etapa 2**, quando houver código
> compilável. O ambiente de build usa `gcc` (MinGW/MSYS2 no Windows ou GCC no Linux).
