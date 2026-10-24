#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include "modelo.h"
#include "gerador.h"
#include "quicksort.h"
#include "util.h"

/*
 * Versão paralela com PROCESSOS + MEMÓRIA COMPARTILHADA.
 *
 * Estratégia (a mesma das versões sequencial e com threads, para comparação justa):
 *   1. O vetor de produtos é alocado em memória compartilhada (mmap MAP_SHARED |
 *      MAP_ANONYMOUS), visível ao pai e a todos os filhos.
 *   2. O vetor é dividido em `nproc` trechos contíguos.
 *   3. Cada processo filho GERA e ORDENA (quicksort) o seu trecho, de forma
 *      independente (trechos disjuntos => sem conflito).
 *   4. O pai aguarda todos os filhos (wait => barreira/sincronização).
 *   5. O pai mescla os `nproc` trechos ordenados (mesclagem k-vias com min-heap).
 *
 * O número de processos é passado em tempo de execução (--proc), nunca fixo.
 */

static void uso(const char *prog) {
    fprintf(stderr,
        "Uso: %s [--lojas N] [--por-loja M] [--seed S] [--proc P] [--saida ARQ]\n"
        "  --proc P       numero de processos (padrao 4)\n",
        prog);
}

int main(int argc, char **argv) {
    uint32_t num_lojas = NUM_LOJAS_PADRAO;
    uint64_t por_loja  = PRODUTOS_POR_LOJA_PADRAO;
    uint64_t seed      = 0;
    int      nproc     = 4;
    const char *saida  = NULL;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--lojas") && i + 1 < argc) num_lojas = (uint32_t)strtoul(argv[++i], NULL, 10);
        else if (!strcmp(argv[i], "--por-loja") && i + 1 < argc) por_loja = strtoull(argv[++i], NULL, 10);
        else if (!strcmp(argv[i], "--seed") && i + 1 < argc) seed = strtoull(argv[++i], NULL, 10);
        else if (!strcmp(argv[i], "--proc") && i + 1 < argc) nproc = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--saida") && i + 1 < argc) saida = argv[++i];
        else if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) { uso(argv[0]); return 0; }
        else { fprintf(stderr, "Argumento desconhecido: %s\n", argv[i]); uso(argv[0]); return 1; }
    }

    if (num_lojas == 0 || por_loja == 0 || nproc < 1) {
        fprintf(stderr, "Erro: --lojas e --por-loja devem ser >= 1; --proc >= 1.\n");
        return 1;
    }

    uint64_t total = (uint64_t)num_lojas * por_loja;

    /* Evita criar mais processos do que elementos. */
    if ((uint64_t)nproc > total) nproc = (int)total;
    if (nproc < 1) nproc = 1;

    fprintf(stderr, "[processos] lojas=%u por-loja=%llu total=%llu seed=%llu proc=%d\n",
            num_lojas, (unsigned long long)por_loja,
            (unsigned long long)total, (unsigned long long)seed, nproc);

    /* Memória compartilhada entre pai e filhos. */
    size_t bytes = (size_t)total * sizeof(Produto);
    Produto *v = mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (v == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    /* Fronteiras dos trechos: o trecho do processo i é [inicios[i], inicios[i+1]). */
    uint64_t *inicios = (uint64_t *)malloc((size_t)(nproc + 1) * sizeof(uint64_t));
    if (inicios == NULL) {
        fprintf(stderr, "Erro: falha ao alocar fronteiras.\n");
        return 1;
    }
    for (int i = 0; i <= nproc; i++) inicios[i] = (uint64_t)i * total / nproc;

    /* Fase paralela: cada filho gera + ordena o seu trecho. */
    double t0 = agora_segundos();
    for (int i = 0; i < nproc; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return 1;
        }
        if (pid == 0) {
            uint64_t ini = inicios[i], fim = inicios[i + 1];
            if (ini < fim) {
                gerar_faixa(v, ini, fim, seed, por_loja);
                quicksort_produtos(&v[ini], (size_t)(fim - ini));
            }
            _exit(0);
        }
    }
    /* Pai aguarda todos os filhos (sincronização). */
    for (int i = 0; i < nproc; i++) wait(NULL);
    double t_paralelo = agora_segundos() - t0;

    /* Mesclagem k-vias (sequencial, no pai). */
    Produto *mesclado = (Produto *)malloc(bytes);
    if (mesclado == NULL) {
        fprintf(stderr, "Erro: falha ao alocar buffer de mesclagem.\n");
        return 1;
    }
    t0 = agora_segundos();
    mesclar_k_vias(mesclado, v, nproc, inicios);
    double t_merge = agora_segundos() - t0;

    munmap(v, bytes);
    free(inicios);

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

    printf("Resultados (processos):\n");
    printf("  processos           : %d\n", nproc);
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
