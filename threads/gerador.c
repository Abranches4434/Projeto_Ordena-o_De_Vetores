#include <stdlib.h>
#include <stdint.h>
#include "gerador.h"

/*
 * Splitmix64: função de hash/finalizador determinística. Ao contrário de um
 * PRNG sequencial (estado que avança), o valor depende apenas da entrada, o que
 * torna a geração independente da ordem — requisito para gerar em paralelo com
 * o mesmo resultado.
 */
static uint64_t splitmix64(uint64_t x) {
    x += 0x9E3779B97F4A7C15ULL;
    x = (x ^ (x >> 30)) * 0xBF58476D1CE4E5B9ULL;
    x = (x ^ (x >> 27)) * 0x94D049BB133111EBULL;
    return x ^ (x >> 31);
}

void gerar_faixa(Produto *v, uint64_t inicio, uint64_t fim, uint64_t seed, uint64_t por_loja) {
    /* Faixa de timestamps: de 2020-01-01 a 2026-01-01 (segundos Unix). */
    const uint64_t T_INICIO = 1577836800ULL;
    const uint64_t T_FIM    = 1767225600ULL;

    for (uint64_t idx = inicio; idx < fim; idx++) {
        uint64_t r0 = splitmix64(seed ^ (idx * 0x9E3779B97F4A7C15ULL));
        uint64_t r1 = splitmix64(r0);
        uint64_t r2 = splitmix64(r1);

        v[idx].id         = idx;                          /* único global */
        v[idx].loja_id    = (uint32_t)(idx / por_loja) + 1;
        v[idx].preco      = (double)(r0 % 1000000) / 100.0; /* R$ 0,00..9999,99 */
        v[idx].data       = T_INICIO + (r1 % (T_FIM - T_INICIO));
        v[idx].quantidade = (uint32_t)(r2 % 1000);
    }
}

Produto *gerar_produtos(uint32_t num_lojas, uint64_t por_loja, uint64_t seed) {
    uint64_t total = (uint64_t)num_lojas * por_loja;
    Produto *v = (Produto *)malloc((size_t)total * sizeof(Produto));
    if (v == NULL) return NULL;
    gerar_faixa(v, 0, total, seed, por_loja);
    return v;
}
