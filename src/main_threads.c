#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include "modelo.h"
#include "gerador.h"
#include "quicksort.h"
#include "util.h"

/*
 * Versão paralela com THREADS (pthreads).
 *
 * Estratégia idêntica à versão com processos (para comparação justa):
 *   1. O vetor de produtos é alocado com malloc — as threads compartilham o
 *      mesmo espaço de endereçamento, então NÃO é preciso mmap (diferença
 *      fundamental em relação aos processos).
 *   2. O vetor é dividido em `nthreads` trechos contíguos (disjuntos).
 *   3. Cada thread GERA e ORDENA (quicksort) o seu trecho.
 *   4. A thread principal aguarda todas (pthread_join => barreira/sincronização).
 *   5. A thread principal mescla os trechos ordenados (mesclagem k-vias).
 *
 * Como os trechos são disjuntos, cada thread escreve apenas na sua região, sem
 * data race e sem necessidade de locks. A sincronização fica por conta do join.
 */

typedef struct {
    Produto *v;
    uint64_t inicio;
    uint64_t fim;
    uint64_t seed;
    uint64_t por_loja;
} ArgsThread;

static void *trabalho_thread(void *arg) {
    ArgsThread *a = (ArgsThread *)arg;
    gerar_faixa(a->v, a->inicio, a->fim, a->seed, a->por_loja);
    quicksort_produtos(&a->v[a->inicio], (size_t)(a->fim - a->inicio));
    return NULL;
}

static void uso(const char *prog) {
    fprintf(stderr,
        "Uso: %s [--lojas N] [--por-loja M] [--seed S] [--threads T] [--saida ARQ]\n"
        "  --threads T    numero de threads (padrao 4)\n",
        prog);
}

int main(int argc, char **argv) {
    uint32_t num_lojas = NUM_LOJAS_PADRAO;
    uint64_t por_loja  = PRODUTOS_POR_LOJA_PADRAO;
    uint64_t seed      = 0;
    int      nthreads  = 4;
    const char *saida  = NULL;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--lojas") && i + 1 < argc) num_lojas = (uint32_t)strtoul(argv[++i], NULL, 10);
        else if (!strcmp(argv[i], "--por-loja") && i + 1 < argc) por_loja = strtoull(argv[++i], NULL, 10);
        else if (!strcmp(argv[i], "--seed") && i + 1 < argc) seed = strtoull(argv[++i], NULL, 10);
        else if (!strcmp(argv[i], "--threads") && i + 1 < argc) nthreads = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--saida") && i + 1 < argc) saida = argv[++i];
        else if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) { uso(argv[0]); return 0; }
        else { fprintf(stderr, "Argumento desconhecido: %s\n", argv[i]); uso(argv[0]); return 1; }
    }

    if (num_lojas == 0 || por_loja == 0 || nthreads < 1) {
        fprintf(stderr, "Erro: --lojas e --por-loja devem ser >= 1; --threads >= 1.\n");
        return 1;
    }

    uint64_t total = (uint64_t)num_lojas * por_loja;

    if ((uint64_t)nthreads > total) nthreads = (int)total;
    if (nthreads < 1) nthreads = 1;

    fprintf(stderr, "[threads] lojas=%u por-loja=%llu total=%llu seed=%llu threads=%d\n",
            num_lojas, (unsigned long long)por_loja,
            (unsigned long long)total, (unsigned long long)seed, nthreads);

    /* Vetor alocado normalmente; threads compartilham o mesmo espaço de memória. */
    size_t bytes = (size_t)total * sizeof(Produto);
    Produto *v = (Produto *)malloc(bytes);
    if (v == NULL) {
        fprintf(stderr, "Erro: falha ao alocar memoria para %llu produtos.\n",
                (unsigned long long)total);
        return 1;
    }

    uint64_t *inicios = (uint64_t *)malloc((size_t)(nthreads + 1) * sizeof(uint64_t));
    pthread_t *tids   = (pthread_t *)malloc((size_t)nthreads * sizeof(pthread_t));
    ArgsThread *args  = (ArgsThread *)malloc((size_t)nthreads * sizeof(ArgsThread));
    if (inicios == NULL || tids == NULL || args == NULL) {
        fprintf(stderr, "Erro: falha ao alocar estruturas das threads.\n");
        return 1;
    }
    for (int i = 0; i <= nthreads; i++) inicios[i] = (uint64_t)i * total / nthreads;

    /* Fase paralela: cada thread gera + ordena o seu trecho. */
    double t0 = agora_segundos();
    for (int i = 0; i < nthreads; i++) {
        args[i].v        = v;
        args[i].inicio   = inicios[i];
        args[i].fim      = inicios[i + 1];
        args[i].seed     = seed;
        args[i].por_loja = por_loja;
        if (pthread_create(&tids[i], NULL, trabalho_thread, &args[i]) != 0) {
            fprintf(stderr, "Erro: pthread_create falhou na thread %d.\n", i);
            return 1;
        }
    }
    /* Thread principal aguarda todas (sincronização). */
    for (int i = 0; i < nthreads; i++) pthread_join(tids[i], NULL);
    double t_paralelo = agora_segundos() - t0;

    /* Mesclagem k-vias (sequencial, na thread principal). */
    Produto *mesclado = (Produto *)malloc(bytes);
    if (mesclado == NULL) {
        fprintf(stderr, "Erro: falha ao alocar buffer de mesclagem.\n");
        return 1;
    }
    t0 = agora_segundos();
    mesclar_k_vias(mesclado, v, nthreads, inicios);
    double t_merge = agora_segundos() - t0;

    free(v);
    free(inicios);
    free(tids);
    free(args);

    int ordenado = verificar_ordenado(mesclado, (size_t)total);

    /* Fase de saída. */
    t0 = agora_segundos();
    if (saida != NULL) {
        FILE *f = fopen(saida, "wb");
        if (f == NULL) {
            fprintf(stderr, "Erro: nao foi possivel abrir '%s'.\n", saida);
            free(mesclado);
            return 1;
        }
        size_t escritos = fwrite(mesclado, sizeof(Produto), (size_t)total, f);
        fclose(f);
        if (escritos != (size_t)total)
            fprintf(stderr, "Aviso: gravou %zu de %llu registros.\n",
                    escritos, (unsigned long long)total);
    }
    uint64_t ck = checksum_produtos(mesclado, (size_t)total);
    double t_saida = agora_segundos() - t0;

    double t_total = t_paralelo + t_merge + t_saida;

    printf("Resultados (threads):\n");
    printf("  threads             : %d\n", nthreads);
    printf("  total de produtos   : %llu\n", (unsigned long long)total);
    printf("  ordenacao verificada: %s\n", ordenado ? "OK" : "FALHA");
    printf("  checksum            : %016llx\n", (unsigned long long)ck);
    printf("  tempo geracao+ord.  : %.3f s  (paralelo)\n", t_paralelo);
    printf("  tempo mesclagem     : %.3f s\n", t_merge);
    printf("  tempo saida         : %.3f s\n", t_saida);
    printf("  tempo total         : %.3f s\n", t_total);
    printf("  primeiros 3:\n");
    for (size_t i = 0; i < 3 && i < (size_t)total; i++) imprimir_produto(&mesclado[i]);
    printf("  ultimos 3:\n");
    for (size_t i = (size_t)total; i > 0 && i > (size_t)total - 3; i--)
        imprimir_produto(&mesclado[i - 1]);

    free(mesclado);
    return 0;
}
