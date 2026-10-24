# Versão Sequencial

Ordena produtos de múltiplas lojas fictícias por **data → valor → id**, usando
**Quicksort** (mediana-de-três + insertion sort para partições pequenas).

## Compilar

```bash
make
```

## Executar

```bash
./sequencial
```

Parâmetros opcionais:

| Parâmetro | Descrição | Padrão |
|-----------|-----------|--------|
| `--lojas N` | quantidade de lojas | 5 |
| `--por-loja M` | produtos por loja | 2.000.000 |
| `--seed S` | semente do gerador | 0 |
| `--saida ARQ` | grava o vetor ordenado em arquivo binário | — |
| `--help` | mostra a ajuda | — |

O programa gera os produtos, ordena e imprime os tempos de cada fase, o checksum
(FNV-1a, usado para validar equivalência entre versões) e a verificação de ordenação.
Com os valores padrão, a execução total fica em torno de **12 s** (≥ 2 s exigido).
