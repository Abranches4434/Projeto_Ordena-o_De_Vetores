#ifndef GERADOR_H
#define GERADOR_H

#include <stdint.h>
#include "modelo.h"

/*
 * Aloca e gera o vetor completo de produtos (todas as lojas).
 * Conveniente para a versão sequencial.
 */
Produto *gerar_produtos(uint32_t num_lojas, uint64_t por_loja, uint64_t seed);

/*
 * Gera os produtos de índice [inicio, fim) no vetor `v` (já alocado).
 *
 * A geração é determinística POR ÍNDICE (splitmix64): o valor de cada produto
 * depende apenas de (seed, índice). Isso permite que processos/threads gerem
 * faixas disjuntas em paralelo produzindo EXATAMENTE os mesmos dados da versão
 * sequencial, independentemente de como o trabalho é dividido.
 */
void gerar_faixa(Produto *v, uint64_t inicio, uint64_t fim, uint64_t seed, uint64_t por_loja);

#endif /* GERADOR_H */
