#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "util.h"

double agora_segundos(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

uint64_t checksum_produtos(const Produto *v, size_t n) {
    const uint64_t PRIME = 1099511628211ULL;
    uint64_t h = 1469598103934665603ULL; /* offset basis FNV-1a */
    for (size_t i = 0; i < n; i++) {
        uint64_t bits_preco;
        memcpy(&bits_preco, &v[i].preco, sizeof(bits_preco));
        h ^= v[i].id;         h *= PRIME;
        h ^= v[i].loja_id;    h *= PRIME;
        h ^= bits_preco;      h *= PRIME;
        h ^= v[i].data;       h *= PRIME;
        h ^= v[i].quantidade; h *= PRIME;
    }
    return h;
}

int verificar_ordenado(const Produto *v, size_t n) {
    for (size_t i = 1; i < n; i++) {
        if (compara_produto(&v[i - 1], &v[i]) > 0) {
            fprintf(stderr, "FALHA: v[%zu] > v[%zu]\n", i - 1, i);
            return 0;
        }
    }
    return 1;
}

void imprimir_produto(const Produto *p) {
    printf("  { id=%llu loja=%u preco=%.2f data=%llu qtd=%u }\n",
           (unsigned long long)p->id,
           p->loja_id,
           p->preco,
           (unsigned long long)p->data,
           p->quantidade);
}

/* --- mesclagem k-vias com min-heap --- */

typedef struct {
    uint64_t pos;   /* índice absoluto do próximo elemento dentro de src */
    uint64_t fim;   /* fim do trecho (exclusivo) */
} NoHeap;

/* Desce o elemento i no min-heap. Compara apenas os valores apontados, sem
   copiar structs Produto inteiras (que custariam caro). */
static void heap_sift_down(NoHeap *h, int n, int i, const Produto *src) {
    for (;;) {
        int l = 2 * i + 1, r = 2 * i + 2, menor = i;
        if (l < n && compara_produto(&src[h[l].pos], &src[h[menor].pos]) < 0) menor = l;
        if (r < n && compara_produto(&src[h[r].pos], &src[h[menor].pos]) < 0) menor = r;
        if (menor == i) break;
        NoHeap tmp = h[i]; h[i] = h[menor]; h[menor] = tmp;
        i = menor;
    }
}

void mesclar_k_vias(Produto *dst, const Produto *src, int k, const uint64_t *inicios) {
    NoHeap *h = (NoHeap *)malloc((size_t)k * sizeof(NoHeap));
    if (h == NULL) {
        fprintf(stderr, "Erro: falha ao alocar heap de mesclagem.\n");
        exit(1);
    }

    int n = 0;
    for (int i = 0; i < k; i++) {
        if (inicios[i] < inicios[i + 1]) {
            h[n].pos = inicios[i];
            h[n].fim = inicios[i + 1];
            n++;
        }
    }
    for (int i = n / 2 - 1; i >= 0; i--) heap_sift_down(h, n, i, src);

    uint64_t out = 0;
    while (n > 0) {
        dst[out++] = src[h[0].pos];
        h[0].pos++;
        if (h[0].pos >= h[0].fim) {
            h[0] = h[n - 1];
            n--;
        }
        heap_sift_down(h, n, 0, src);
    }

    free(h);
}
