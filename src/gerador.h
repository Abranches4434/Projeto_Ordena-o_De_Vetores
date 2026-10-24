#ifndef GERADOR_H
#define GERADOR_H

#include <stdint.h>
#include "modelo.h"

/*
 * Gera o vetor de produtos de todas as lojas.
 *
 * - num_lojas: quantidade de lojas (>= 1)
 * - por_loja:  produtos por loja (>= 1)
 * - seed:      semente do gerador pseudoaleatorio (deterministico)
 *
 * Os produtos sao agrupados por loja: os primeiros `por_loja` sao da loja 1,
 * os seguintes da loja 2, e assim por diante. O campo `id` e o indice global,
 * garantindo unicidade e servindo de desempate estavel na ordenacao.
 *
 * Retorna ponteiro para o vetor alocado com malloc(), ou NULL em caso de erro.
 * O chamador e responsavel por liberar com free().
 */
Produto *gerar_produtos(uint32_t num_lojas, uint64_t por_loja, uint64_t seed);

#endif /* GERADOR_H */
