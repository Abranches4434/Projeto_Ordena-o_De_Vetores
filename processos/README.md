# Versão com Processos + Memória Compartilhada

Paraleliza **geração** e **ordenação** usando múltiplos processos (`fork`) com
**memória compartilhada** (`mmap MAP_SHARED | MAP_ANONYMOUS`).

O vetor de produtos é dividido em `P` trechos; cada processo filho gera e ordena
o seu trecho com Quicksort; o pai aguarda (`wait`) e mescla os trechos ordenados
(mesclagem k-vias com min-heap).

## Compilar

```bash
make
```

## Executar

```bash
./processos --proc 4
```

| Parâmetro | Descrição | Padrão |
|-----------|-----------|--------|
| `--proc P` | número de processos | 4 |
| `--lojas N` | quantidade de lojas | 5 |
| `--por-loja M` | produtos por loja | 2.000.000 |
| `--seed S` | semente do gerador | 0 |
| `--saida ARQ` | grava o vetor ordenado em arquivo binário | — |

> O número de processos é passado em tempo de execução (nunca fixo no código),
> conforme exigido pelo enunciado. O checksum deve ser idêntico ao da versão
> sequencial para a mesma semente.
