#ifndef QUICKSORT_H
#define QUICKSORT_H

#include <stddef.h>
#include "modelo.h"

/*
 * Ordena o vetor `v` de `n` produtos em ordem crescente segundo
 * `compara_produto` (data -> preco -> id).
 *
 * Implementacao recursiva do Quicksort com pivo mediana-de-tres e
 * insertion sort para particoes pequenas. Esta e a versao sequencial;
 * as versoes paralelas (Etapas 3 e 4) reaproveitam a logica de particao.
 */
void quicksort_produtos(Produto *v, size_t n);

#endif /* QUICKSORT_H */
