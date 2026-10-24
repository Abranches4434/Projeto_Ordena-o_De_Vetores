#ifndef MODELO_H
#define MODELO_H

#include <stdint.h>

/*
 * modelo.h — Definição do tipo Produto e dos critérios de ordenação.
 *
 * Este arquivo é a "fonte única de verdade" dos dados do projeto: as três
 * versões (sequencial, processos e threads) usam exatamente a mesma struct e o
 * mesmo comparador, garantindo resultados equivalentes para as mesmas entradas.
 */

/* Quantidade padrão de lojas (pode ser sobrescrita na linha de comando). */
#define NUM_LOJAS_PADRAO 5

/*
 * Quantidade padrão de produtos por loja.
 * 5 lojas x 1.000.000 = 5.000.000 produtos no total, valor usado como ponto de
 * partida para garantir execução sequencial >= 2 segundos. Ajustável.
 */
#define PRODUTOS_POR_LOJA_PADRAO 1000000

typedef struct {
    uint64_t id;         /* identificador único global (usado como desempate)   */
    uint32_t loja_id;    /* 1..N, identifica a loja de origem                   */
    double   preco;      /* valor do produto (R$)                               */
    uint64_t data;       /* timestamp de cadastro (critério primário)           */
    uint32_t quantidade; /* quantidade em estoque                               */
} Produto;

/*
 * Comparador usado pelo quicksort.
 *
 * Ordem de prioridade:
 *   1. data   (crescente)
 *   2. preco  (crescente)
 *   3. id     (crescente — desempate estável)
 *
 * Retorno: < 0 se (a < b), 0 se iguais, > 0 se (a > b).
 */
static inline int compara_produto(const Produto *a, const Produto *b) {
    if (a->data != b->data)   return (a->data < b->data)   ? -1 : 1;
    if (a->preco != b->preco) return (a->preco < b->preco) ? -1 : 1;
    if (a->id != b->id)       return (a->id < b->id)       ? -1 : 1;
    return 0;
}

#endif /* MODELO_H */
