#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <stdint.h>
#include "modelo.h"

/* Tempo monotônico em segundos (fonte única para todas as medições). */
double agora_segundos(void);

/*
 * Checksum FNV-1a sobre os campos (não sobre os bytes crus, para evitar
 * incluir padding não inicializado). Permite comparar a equivalência dos
 * resultados entre as três versões: mesma saída => mesmo checksum.
 */
uint64_t checksum_produtos(const Produto *v, size_t n);

/* Verifica se o vetor está em ordem não-decrescente (evidência de correção). */
int verificar_ordenado(const Produto *v, size_t n);

void imprimir_produto(const Produto *p);

/*
 * Mescla `k` trechos ordenados de `src` (delimitados por inicios[0..k]) em `dst`.
 * Os trechos são as fatias já ordenadas do vetor compartilhado; a mesclagem
 * k-vias com min-heap produz a ordem global.
 */
void mesclar_k_vias(Produto *dst, const Produto *src, int k, const uint64_t *inicios);

#endif /* UTIL_H */
