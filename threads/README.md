# Versão com Threads (pthreads)

Paraleliza **geração** e **ordenação** usando múltiplas threads (`pthreads`),
com a mesma estratégia da versão com processos. As threads **compartilham o heap**
por padrão, então não é preciso `mmap`.

Cada thread gera e ordena um trecho disjunto do vetor (Quicksort); a thread
principal aguarda (`pthread_join`) e mescla os trechos (mesclagem k-vias).

## Compilar

```bash
make
```

## Executar

```bash
./threads --threads 4
```

| Parâmetro | Descrição | Padrão |
|-----------|-----------|--------|
| `--threads T` | número de threads | 4 |
| `--lojas N` | quantidade de lojas | 5 |
| `--por-loja M` | produtos por loja | 2.000.000 |
| `--seed S` | semente do gerador | 0 |
| `--saida ARQ` | grava o vetor ordenado em arquivo binário | — |

> O número de threads é passado em tempo de execução (nunca fixo). O checksum deve
> ser idêntico ao das versões sequencial e com processos para a mesma semente.
