# SPEC.md — Ordenação de Produtos Multi-loja (Trabalho P1 — SO2)

> Documento vivo do projeto. Regras de manutenção (definidas com o aluno):
>
> - **Feature nova** → criar uma nova **seção** com `O que`, `Por que`, `Onde` e `Consequências`.
> - **Alteração em algo já existente** → registrar no **Changelog** (tabela abaixo), apontando a seção afetada.
> - Cada **macro-etapa** de desenvolvimento ganha uma seção própria.

---

## Changelog

| Data       | Etapa   | Tipo         | Descrição |
|------------|---------|--------------|-----------|
| 23/10/2026 | Etapa 1 | Nova feature | Fundação: problema, modelo de dados, arquitetura e estrutura do projeto. |
| 23/10/2026 | Etapa 2 | Nova feature | Versão sequencial: gerador, quicksort e medição de tempo (linha de base). |
| 23/10/2026 | Etapa 3 | Alteração | Gerador refatorado (xorshift64\* → splitmix64 por índice) para permitir geração paralela determinística. |
| 23/10/2026 | Etapa 3 | Alteração | Funções comuns extraídas para `src/util.c/.h` (tempo, checksum, verificação, impressão, mesclagem). |
| 23/10/2026 | Etapa 3 | Nova feature | Versão paralela com processos + memória compartilhada (mmap + fork + mesclagem k-vias). |
| 23/10/2026 | Etapa 4 | Nova feature | Versão paralela com threads (pthreads), mesma estratégia dos processos. |
| 23/10/2026 | Reestruturação | Alteração | Diretórios reorganizados em pastas autocontidas por versão + `container/`/`testes/`/`documentacao/`, conforme enunciado. |
| 23/10/2026 | Etapa 5 | Nova feature | Container Docker, benchmark (nativo + container) e gráficos de tempo/speedup. |

---

## Etapa 1 — Proposta e Modelo de Dados

### O que

Aplicação em **C** que consolida e ordena produtos de **5 ou mais lojas fictícias**
(dados gerados de forma sintética) e os ordena por **data**, **valor** e um
desempate estável por `id`, usando o algoritmo **Quicksort**.

O pipeline é dividido em três fases, todas com potencial de paralelização:

1. **Entrada (geração):** cada loja gera seus produtos de forma independente.
2. **Ordenação:** quicksort sobre o vetor compartilhado de todos os produtos.
3. **Saída:** gravação do vetor ordenado em fatias independentes.

### Por que

Atender ao **Trabalho P1 de SO2 (Programação Paralela)**: implementar uma versão
sequencial e transformá-la em duas versões paralelas — com **processos
(memória compartilhada)** e com **threads** —, comparando-as experimentalmente.

A ordenação de produtos de múltiplas fontes foi escolhida porque:

- é uma *aplicação* realista (agregador de produtos), não um algoritmo isolado;
- permite demonstrar **divisão de trabalho**, **sincronização** e **regiões críticas**
  (requisito explícito do enunciado);
- tem carga facilmente escalável para garantir **≥ 2 segundos** de execução sequencial;
- produz **resultados equivalentes** entre as versões (ordenação determinística).

### Onde

| Arquivo/Diretório        | Responsabilidade |
|--------------------------|------------------|
| `SPEC.md`                | Este documento (especificação viva). |
| `README.md`              | Visão geral e instruções de compilação/execução. |
| `Makefile`               | Build das três versões. |
| `src/modelo.h`           | `struct Produto`, constantes e comparador de ordenação. |
| `src/gerador.c/.h`       | Geração sintética dos produtos por loja (Etapa 2). |
| `src/quicksort.c/.h`     | Quicksort sequencial e paralelo (Etapas 2–4). |
| `src/main_sequencial.c`  | Versão sequencial (Etapa 2). |
| `src/main_processos.c`   | Versão com processos (Etapa 3). |
| `src/main_threads.c`     | Versão com threads (Etapa 4). |
| `testes/entradas/`       | Seeds e configurações das entradas de teste. |
| `testes/resultados/`     | Tempos e speedups medidos. |
| `container/Dockerfile`   | Container da aplicação (Etapa 5). |

### Consequências

- Fixa o **contrato de dados** (`struct Produto`) que as três versões devem respeitar,
  garantindo resultados equivalentes para as mesmas entradas.
- Define os **critérios de ordenação** (data → valor → id), concentrados num único
  comparador (`compara_produto`), evitando divergências entre versões.
- Estabelece a **divisão em três fases**, que serão paralelizadas nas Etapas 3 e 4,
  delimitando os pontos de sincronização (barreiras entre fases e região crítica no merge).
- Permite o requisito de **≥ 2s** ajustando a quantidade de produtos por loja
  (ver `PRODUTOS_POR_LOJA_PADRAO` em `src/modelo.h`).

---

## Etapa 2 — Versão Sequencial

### O que

Implementação da versão sequencial completa do pipeline em **C**:

1. **Geração** (`gerador.c`): cria os produtos das lojas com um PRNG
   determinístico (xorshift64\*), garantindo dados reprodutíveis pela semente.
2. **Ordenação** (`quicksort.c`): Quicksort recursivo com pivô mediana-de-três e
   insertion sort para partições pequenas, usando o comparador de `modelo.h`.
3. **Saída** (`main_sequencial.c`): grava o vetor ordenado (binário, opcional),
   calcula um checksum FNV-1a e imprime os tempos de cada fase.

### Por que

É a base exigida pelo enunciado: uma aplicação sequencial cuja execução leve
pelo menos **2 segundos**, servindo de referência para o cálculo de speedup das
versões paralelas (Etapas 3 e 4).

### Onde

- `src/gerador.c/.h` — geração determinística dos produtos.
- `src/quicksort.c/.h` — Quicksort sequencial (lógica reutilizável nas versões paralelas).
- `src/main_sequencial.c` — ponto de entrada, medição de tempo e checksum.
- `Makefile` — alvo `sequencial`.

### Consequências

- Estabelece a **linha de base** de tempo (`t_seq`) usada no cálculo de speedup.
- Introduz o **checksum FNV-1a** como verificação de equivalência entre versões
  (mesma saída => mesmo checksum).
- O PRNG determinístico permite comparar as versões com exatamente a mesma entrada.

### Validação (medição de referência)

Em WSL2/Ubuntu (gcc 15.2, `-O2`), com a entrada padrão (5 lojas × 2.000.000 =
10.000.000 produtos):

| Fase | Tempo |
|------|-------|
| Geração | ~1,88 s |
| Ordenação | ~5,38 s |
| Saída | ~0,09 s |
| **Total** | **~7,35 s** |

- Ordenação confirmada por varredura O(n) (`ordenacao verificada: OK`).
- Checksum: `730a917b356d1467` (referência para comparar com as versões paralelas).

---

## Etapa 3 — Versão Paralela com Processos

### O que

Versão que paraleliza as fases de **geração** e **ordenação** usando múltiplos
processos (`fork`) com **memória compartilhada** (`mmap` com `MAP_SHARED |
MAP_ANONYMOUS`):

1. O vetor de produtos é alocado em memória compartilhada, visível ao pai e aos filhos.
2. O vetor é dividido em `nproc` trechos contíguos (disjuntos).
3. Cada processo filho gera (`gerar_faixa`) e ordena (quicksort) o seu trecho.
4. O pai aguarda todos os filhos (`wait` — sincronização/barreira).
5. O pai mescla os trechos ordenados (mesclagem k-vias com min-heap).

O número de processos é passado em tempo de execução (`--proc P`), nunca fixo no código.

### Por que

Atende ao requisito do enunciado de uma versão paralela com **múltiplos processos e
memória compartilhada**, demonstrando divisão de trabalho (trechos), comunicação
(via memória compartilhada), sincronização (`wait`) e a fase de coordenação (mesclagem).

### Onde

- `src/main_processos.c` — versão com processos (fork + mmap + wait + merge).
- `src/util.c/.h` — mesclagem k-vias (`mesclar_k_vias`) e funções comuns.
- `src/gerador.c/.h` — `gerar_faixa` (geração determinística por índice, paralelizável).
- `Makefile` — alvo `processos`.

### Consequências

- Introduz **memória compartilhada** via `mmap` (`MAP_SHARED | MAP_ANONYMOUS`) entre pai e filhos.
- A **mesclagem k-vias é sequencial** e vira o gargalo (Lei de Amdahl): quanto mais
  processos, maior o peso relativo da mesclagem, limitando o speedup.
- A geração determinística por índice (`splitmix64`) garante que as faixas geradas em
  paralelo produzam **exatamente** os mesmos dados da versão sequencial.

### Validação (medição preliminar)

Todos os casos produzem `checksum = a07c6fe59a7b186b` (idêntico à versão sequencial) e
`ordenacao verificada: OK`.

| Processos | Ger+Ord (paralelo) | Mesclagem | Total | Speedup\* |
|-----------|--------------------|-----------|-------|-----------|
| 2 | 3,03 s | 4,78 s | 7,92 s | ~1,5x |
| 4 | 4,12 s | 1,98 s | 6,20 s | ~1,9x |
| 8 | 1,65 s | 1,23 s | 2,99 s | ~4,0x |

\*Speedup em relação ao sequencial (~12,05 s). **Valores preliminares de uma única
execução**; a medição rigorosa (múltiplas execuções, média e desvio) será feita na
Etapa 5, pois os tempos variaram bastante entre execuções (carga da máquina).

---

## Etapa 4 — Versão Paralela com Threads

### O que

Versão que paraleliza geração e ordenação usando **múltiplas threads** (`pthreads`),
com a **mesma estratégia** da versão com processos (para comparação justa):

1. O vetor de produtos é alocado com `malloc` — as threads **compartilham o mesmo
   espaço de endereçamento**, então não é preciso `mmap` (diferença central em
   relação aos processos).
2. O vetor é dividido em `nthreads` trechos contíguos (disjuntos).
3. Cada thread gera (`gerar_faixa`) e ordena (quicksort) o seu trecho.
4. A thread principal aguarda todas (`pthread_join` — barreira/sincronização).
5. A thread principal mescla os trechos ordenados (mesclagem k-vias, sequencial).

O número de threads é passado em tempo de execução (`--threads T`), nunca fixo.

### Por que

Atende ao requisito do enunciado de uma versão paralela com **múltiplas threads**,
permitindo comparar diretamente processos × threads sobre o mesmo algoritmo.

### Onde

- `src/main_threads.c` — versão com threads (pthread_create/pthread_join + merge).
- `Makefile` — alvo `threads` (compilado com `-pthread`).

### Consequências

- **Threads compartilham memória por padrão** (heap), dispensando o `mmap` usado na
  versão com processos — ponto-chave na comparação entre os dois modelos.
- Como os trechos são **disjuntos**, cada thread escreve apenas na sua região:
  **não há data race nem locks**; a sincronização fica por conta do `pthread_join`.
- A **mesclagem continua sequencial** (conforme decidido na Etapa 3), preservando o
  gargalo de Amdahl para fins de comparação.

### Validação (medição preliminar)

`checksum = a07c6fe59a7b186b` em 2/4/8 threads — **idêntico** às versões sequencial e
com processos; `ordenacao verificada: OK`.

| Threads | Ger+Ord (paralelo) | Total |
|---------|--------------------|-------|
| 2 | 4,62 s | 5,95 s |
| 4 | 2,32 s | (ruidoso) |
| 8 | 1,77 s | 3,86 s |

A fase paralela (ger+ord) escala com o número de threads; os tempos de mesclagem e
saída tiveram **alta variância entre execuções** (carga da máquina), então a medição
rigorosa (múltiplas execuções, média e desvio) será feita na Etapa 5.

---

## Reorganização da Estrutura de Diretórios

### O que

A árvore do repositório foi reorganizada para refletir exatamente a estrutura de
entrega exigida no enunciado (zip com `/sequencial`, `/processos`, `/threads`,
`/container`, `/testes` e `/documentacao`).

Cada pasta de versão passou a ser **autocontida** (contém seu próprio código-fonte,
`Makefile` e `README.md`), com os arquivos comuns (`modelo.h`, `gerador.*`,
`quicksort.*`, `util.*`) duplicados em cada uma.

### Por que

O enunciado especifica uma estrutura de entrega com uma pasta por versão, cada uma
com "código-fonte" e "README". A organização anterior (um único `src/` compartilhado)
era mais enxuta, mas não correspondia à estrutura exigida.

### Onde

```
├── sequencial/   # main_sequencial.c + arquivos comuns + Makefile + README.md
├── processos/    # main_processos.c  + arquivos comuns + Makefile + README.md
├── threads/      # main_threads.c    + arquivos comuns + Makefile + README.md
├── container/    # Dockerfile (Etapa 5)
├── testes/
│   ├── entradas/
│   └── resultados/
└── documentacao/ # apresentacao.pdf
```

### Consequências

- As referências a `src/...` nas etapas anteriores passaram a apontar para as
  cópias dentro de cada pasta de versão.
- Mudanças futuras no código comum precisam ser aplicadas nas três pastas (custo
  da duplicação, aceito para atender à estrutura do enunciado).

---

## Etapa 5 — Container Docker e Análise de Desempenho

### O que

1. **`container/Dockerfile`** — imagem (ubuntu:24.04 + gcc/make) com as três versões compiladas.
2. **`testes/benchmark.sh`** — benchmark nativo (fora do container), N execuções por configuração.
3. **`testes/benchmark_container.sh`** — benchmark dentro do container com `--cpus=1/2/4`.
4. **`testes/graficos.py`** — gera os 3 gráficos (tempo, speedup, fora × dentro).

### Por que

Fecha os requisitos do enunciado: disponibilizar a aplicação em container, executar
com limites de CPU, medir o speedup e apresentar os resultados graficamente.

### Onde

- `container/Dockerfile`
- `testes/benchmark.sh`, `testes/benchmark_container.sh`, `testes/graficos.py`
- `testes/resultados/*.csv` (dados) e `*.png` (gráficos)

### Resultados (mediana, 5 execuções; máquina com alta variância)

`checksum` idêntico em todas as configurações (nativo e container): equivalência confirmada.

**Tempo total e speedup (nativo, sequencial = 5,08 s):**

| Workers | Processos | Speedup | Threads | Speedup |
|---------|-----------|---------|---------|---------|
| 2 | 5,93 s | 0,86x | 4,15 s | 1,22x |
| 4 | 5,38 s | 0,94x | 3,73 s | 1,36x |
| 8 | 4,25 s | 1,19x | 2,92 s | 1,74x |

**Efeito do limite de CPU (container, exemplo processos com 8 workers):**

| CPU | Tempo |
|-----|-------|
| 1 | 10,18 s |
| 2 | 6,86 s |
| 4 | 4,18 s |

### Consequências / Interpretação

- **Threads escalam melhor que processos** (criação de thread é mais barata que `fork`
  + `mmap`), chegando a ~1,7x com 8 threads.
- **Processos têm pouco ganho (ou perda) nesta escala**: o custo de `fork`/`mmap` somado
  à mesclagem sequencial supera o ganho da ordenação paralela em 2–4 processos.
- A **mesclagem sequencial** é o gargalo (Lei de Amdahl), limitando o speedup de ambos.
- A máquina apresentou **alta variância** (oscilações de ~3× entre execuções), então
  usou-se a **mediana** e os valores são aproximados; em um ambiente controlado o
  speedup tende a ser maior.
