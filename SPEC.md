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

## Próximas etapas (planejadas)

- **Etapa 2** — Versão sequencial completa (geração + quicksort + saída), com medição ≥ 2s.
- **Etapa 3** — Versão paralela com processos (`fork` + `mmap`).
- **Etapa 4** — Versão paralela com threads (`pthreads`).
- **Etapa 5** — Container Docker + limites de CPU + gráficos de tempo/speedup.
