#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "modelo.h"
#include "gerador.h"
#include "quicksort.h"
#include "util.h"

static void uso(const char *prog) {
    fprintf(stderr,
        "Uso: %s [--lojas N] [--por-loja M] [--seed S] [--saida ARQ]\n"
        "  --lojas N      quantidade de lojas (padrao %u)\n"
        "  --por-loja M   produtos por loja (padrao %llu)\n"
        "  --seed S       semente do gerador (padrao 0)\n"
        "  --saida ARQ    grava o vetor ordenado em ARQ (binario)\n",
        prog, NUM_LOJAS_PADRAO, (unsigned long long)PRODUTOS_POR_LOJA_PADRAO);
}

int main(int argc, char **argv) {
    uint32_t num_lojas = NUM_LOJAS_PADRAO;
    uint64_t por_loja  = PRODUTOS_POR_LOJA_PADRAO;
    uint64_t seed      = 0;
    const char *saida  = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--lojas") == 0 && i + 1 < argc) {
            num_lojas = (uint32_t)strtoul(argv[++i], NULL, 10);
        } else if (strcmp(argv[i], "--por-loja") == 0 && i + 1 < argc) {
            por_loja = strtoull(argv[++i], NULL, 10);
        } else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            seed = strtoull(argv[++i], NULL, 10);
        } else if (strcmp(argv[i], "--saida") == 0 && i + 1 < argc) {
            saida = argv[++i];
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            uso(argv[0]);
            return 0;
        } else {
            fprintf(stderr, "Argumento desconhecido: %s\n", argv[i]);
            uso(argv[0]);
            return 1;
        }
    }

    if (num_lojas == 0 || por_loja == 0) {
        fprintf(stderr, "Erro: --lojas e --por-loja devem ser >= 1.\n");
        return 1;
    }

    uint64_t total = (uint64_t)num_lojas * por_loja;
    fprintf(stderr, "[sequencial] lojas=%u por-loja=%llu total=%llu seed=%llu\n",
            num_lojas, (unsigned long long)por_loja,
            (unsigned long long)total, (unsigned long long)seed);

    /* Fase 1: entrada (geração). */
    double t0 = agora_segundos();
    Produto *v = gerar_produtos(num_lojas, por_loja, seed);
    double t1 = agora_segundos();
    if (v == NULL) {
        fprintf(stderr, "Erro: falha ao alocar memoria para %llu produtos.\n",
                (unsigned long long)total);
        return 1;
    }
    double t_geracao = t1 - t0;

    /* Fase 2: ordenação. */
    t0 = agora_segundos();
    quicksort_produtos(v, (size_t)total);
    t1 = agora_segundos();
    double t_ordenacao = t1 - t0;

    int ordenado = verificar_ordenado(v, (size_t)total);

    /* Fase 3: saída. */
    t0 = agora_segundos();
    if (saida != NULL) {
        FILE *f = fopen(saida, "wb");
        if (f == NULL) {
            fprintf(stderr, "Erro: nao foi possivel abrir '%s' para escrita.\n", saida);
            free(v);
            return 1;
        }
        size_t escritos = fwrite(v, sizeof(Produto), (size_t)total, f);
        fclose(f);
        if (escritos != (size_t)total)
            fprintf(stderr, "Aviso: gravou %zu de %llu registros.\n",
                    escritos, (unsigned long long)total);
    }
    uint64_t ck = checksum_produtos(v, (size_t)total);
    t1 = agora_segundos();
    double t_saida = t1 - t0;

    double t_total = t_geracao + t_ordenacao + t_saida;

    printf("Resultados (sequencial):\n");
    printf("  total de produtos  : %llu\n", (unsigned long long)total);
    printf("  ordenacao verificada: %s\n", ordenado ? "OK" : "FALHA");
    printf("  checksum           : %016llx\n", (unsigned long long)ck);
    printf("  tempo geracao      : %.3f s\n", t_geracao);
    printf("  tempo ordenacao    : %.3f s\n", t_ordenacao);
    printf("  tempo saida        : %.3f s\n", t_saida);
    printf("  tempo total        : %.3f s\n", t_total);
    printf("  primeiros 3:\n");
    for (size_t i = 0; i < 3 && i < (size_t)total; i++) imprimir_produto(&v[i]);
    printf("  ultimos 3:\n");
    for (size_t i = (size_t)total; i > 0 && i > (size_t)total - 3; i--)
        imprimir_produto(&v[i - 1]);

    free(v);
    return 0;
}
