#include "quicksort.h"


#define CORTE_INSERTION 16

static void troca(Produto *a, Produto *b) {
    Produto t = *a;
    *a = *b;
    *b = t;
}


static Produto *mediana_de_tres(Produto *a, Produto *b, Produto *c) {
    if (compara_produto(a, b) < 0) {
        if (compara_produto(b, c) < 0) return b;
        if (compara_produto(a, c) < 0) return c;
        return a;
    } else {
        if (compara_produto(a, c) < 0) return a;
        if (compara_produto(b, c) < 0) return c;
        return b;
    }
}


static size_t particionar(Produto *v, size_t lo, size_t hi) {
    size_t mid = lo + (hi - lo) / 2;
    Produto *p = mediana_de_tres(&v[lo], &v[mid], &v[hi]);
    troca(p, &v[hi]);
    Produto *pivo = &v[hi];

    size_t i = lo;
    for (size_t j = lo; j < hi; j++) {
        if (compara_produto(&v[j], pivo) < 0) {
            troca(&v[i], &v[j]);
            i++;
        }
    }
    troca(&v[i], &v[hi]);
    return i;
}

static void insertion_sort(Produto *v, size_t lo, size_t hi) {
    for (size_t i = lo + 1; i <= hi; i++) {
        Produto chave = v[i];
        size_t j = i;
        while (j > lo && compara_produto(&chave, &v[j - 1]) < 0) {
            v[j] = v[j - 1];
            j--;
        }
        v[j] = chave;
    }
}

static void quicksort_rec(Produto *v, size_t lo, size_t hi) {
    if (lo >= hi) return;
    if (hi - lo < CORTE_INSERTION) {
        insertion_sort(v, lo, hi);
        return;
    }
    size_t p = particionar(v, lo, hi);
    if (p > 0) quicksort_rec(v, lo, p - 1);
    quicksort_rec(v, p + 1, hi);
}

void quicksort_produtos(Produto *v, size_t n) {
    if (n < 2) return;
    quicksort_rec(v, 0, n - 1);
}
