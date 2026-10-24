#!/usr/bin/env python3
"""
Gera os gráficos do Trabalho P1 — SO2 a partir dos CSVs de benchmark.

Requer: matplotlib (pip install matplotlib).

Entradas (em testes/resultados/):
  - tempos.csv           (benchmark nativo)
  - tempos_container.csv (benchmark dentro do container)

Saídas (em testes/resultados/):
  - grafico1_tempo.png
  - grafico2_speedup.png
  - grafico3_fora_dentro.png
"""

import csv
import os
from collections import defaultdict
import statistics as st

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

BASE = os.path.dirname(os.path.abspath(__file__))
RES = os.path.join(BASE, "resultados")


def ler_tempos(caminho):
    """Retorna dict {(versao, workers): [tempos...]}."""
    dados = defaultdict(list)
    with open(caminho, newline="", encoding="utf-8") as f:
        for linha in csv.DictReader(f):
            chave = (linha["versao"], int(linha["workers"]))
            dados[chave].append(float(linha["tempo_total"]))
    return dados


def ler_container(caminho):
    """Retorna dict {(versao, workers): {cpus: tempo}} (última medição)."""
    dados = defaultdict(dict)
    with open(caminho, newline="", encoding="utf-8") as f:
        for linha in csv.DictReader(f):
            chave = (linha["versao"], int(linha["workers"]))
            dados[chave][int(linha["cpus"])] = float(linha["tempo_total"])
    return dados


def media(dados, versao, workers):
    """Mediana (robusta a outliers de carga da máquina)."""
    vals = dados.get((versao, workers), [])
    return st.median(vals) if vals else None


def main():
    plt.rcParams.update({"font.size": 11, "figure.dpi": 110})

    nat_path = os.path.join(RES, "tempos.csv")
    cont_path = os.path.join(RES, "tempos_container.csv")

    nat = ler_tempos(nat_path) if os.path.exists(nat_path) else {}
    cont = ler_container(cont_path) if os.path.exists(cont_path) else {}

    t_seq = media(nat, "sequencial", 1)
    workers = [2, 4, 8]

    # ------------------------------------------------------------------
    # Gráfico 1 — Tempo de execução
    # ------------------------------------------------------------------
    fig, ax = plt.subplots(figsize=(8, 5))
    if t_seq is not None:
        ax.axhline(t_seq, color="gray", linestyle="--", label=f"Sequencial ({t_seq:.2f} s)")
    for versao, cor, marc in [("processos", "#1f77b4", "o"), ("threads", "#2ca02c", "s")]:
        ys = [media(nat, versao, w) for w in workers]
        ax.plot(workers, ys, marker=marc, color=cor, label=versao.capitalize())
    ax.set_xlabel("Nº de processos/threads")
    ax.set_ylabel("Tempo total (s)")
    ax.set_title("Gráfico 1 — Tempo de execução")
    ax.set_xticks([1, 2, 4, 8])
    ax.grid(True, alpha=0.3)
    ax.legend()
    fig.tight_layout()
    fig.savefig(os.path.join(RES, "grafico1_tempo.png"))
    plt.close(fig)

    # ------------------------------------------------------------------
    # Gráfico 2 — Speedup
    # ------------------------------------------------------------------
    fig, ax = plt.subplots(figsize=(8, 5))
    if t_seq is not None:
        ax.axline((0, 0), slope=1, color="gray", linestyle=":", label="Speedup ideal (=workers)")
        for versao, cor, marc in [("processos", "#1f77b4", "o"), ("threads", "#2ca02c", "s")]:
            sp = []
            for w in workers:
                t = media(nat, versao, w)
                sp.append((t_seq / t) if t else None)
            ax.plot(workers, sp, marker=marc, color=cor, label=versao.capitalize())
    ax.set_xlabel("Nº de processos/threads")
    ax.set_ylabel("Speedup (t_seq / t_par)")
    ax.set_title("Gráfico 2 — Speedup")
    ax.set_xticks([0, 1, 2, 4, 8])
    ax.grid(True, alpha=0.3)
    ax.legend()
    fig.tight_layout()
    fig.savefig(os.path.join(RES, "grafico2_speedup.png"))
    plt.close(fig)

    # ------------------------------------------------------------------
    # Gráfico 3 — Fora do container X Dentro do container
    # ------------------------------------------------------------------
    if cont:
        fig, axes = plt.subplots(1, 2, figsize=(12, 5), sharey=True)
        for ax, versao, cor in zip(axes, ["processos", "threads"], ["#1f77b4", "#2ca02c"]):
            x = [0, 1, 2, 3]
            largura = 0.18
            for i, w in enumerate(workers):
                fora = media(nat, versao, w)
                dentro = cont.get((versao, w), {})
                val = [fora] + [dentro.get(c, None) for c in (1, 2, 4)]
                pos = [xi + i * largura for xi in x]
                ax.bar(pos, val, width=largura, label=f"{w} workers")
            ax.set_xticks([xi + largura for xi in x])
            ax.set_xticklabels(["Fora", "CPU=1", "CPU=2", "CPU=4"])
            ax.set_xlabel("Ambiente de execução")
            ax.set_title(f"Gráfico 3 — {versao.capitalize()}: fora X dentro")
            ax.grid(True, axis="y", alpha=0.3)
            ax.legend()
        axes[0].set_ylabel("Tempo total (s)")
        fig.tight_layout()
        fig.savefig(os.path.join(RES, "grafico3_fora_dentro.png"))
        plt.close(fig)

    print("Gráficos gerados em", RES)


if __name__ == "__main__":
    main()
