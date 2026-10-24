#include <stdlib.h>
#include <stdint.h>
#include "gerador.h"

/*
 * PRNG deterministico xorshift64*.
 * Mesma semente => mesmos dados em qualquer versao (sequencial/processos/threads),
 * o que garante a comparacao justa e a equivalencia de resultados.
 */
static uint64_t xorshift64(uint64_t *s) {
    uint64_t x = *s;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    *s = x;
    return x;
}

Produto *gerar_produtos(uint32_t num_lojas, uint64_t por_loja, uint64_t seed) {
    uint64_t total = (uint64_t)num_lojas * por_loja;
    Produto *v = (Produto *)malloc((size_t)total * sizeof(Produto));
    if (v == NULL) return NULL;

    uint64_t rng = seed ? seed : 0x9E3779B97F4A7C15ULL;

    /* Faixa de timestamps: de 2020-01-01 a 2026-01-01 (segundos Unix). */
    const uint64_t T_INICIO = 1577836800ULL;
    const uint64_t T_FIM    = 1767225600ULL;

    uint64_t idx = 0;
    for (uint32_t loja = 0; loja < num_lojas; loja++) {
        for (uint64_t j = 0; j < por_loja; j++) {
            v[idx].id         = idx;                          /* unico global */
            v[idx].loja_id    = loja + 1;
            v[idx].preco      = (double)(xorshift64(&rng) % 1000000) / 100.0; /* R$ 0,00..9999,99 */
            v[idx].data       = T_INICIO + (xorshift64(&rng) % (T_FIM - T_INICIO));
            v[idx].quantidade = (uint32_t)(xorshift64(&rng) % 1000);
            idx++;
        }
    }
    return v;
}
