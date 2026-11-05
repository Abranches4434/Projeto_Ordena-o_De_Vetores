#!/usr/bin/env python3
"""
Gera os slides (PowerPoint) de apresentação do Trabalho P1 — SO2.

Requer: python-pptx (pip install python-pptx).

Entrada: os gráficos em testes/resultados/*.png
Saída:   documentacao/apresentacao.pptx
"""

import os
from pptx import Presentation
from pptx.util import Inches, Pt
from pptx.dml.color import RGBColor
from pptx.enum.text import PP_ALIGN

BASE = os.path.dirname(os.path.abspath(__file__))
RAIZ = os.path.dirname(BASE)
RES = os.path.join(BASE, "resultados")
DOC = os.path.join(RAIZ, "documentacao")
os.makedirs(DOC, exist_ok=True)

G1 = os.path.join(RES, "grafico1_tempo.png")
G2 = os.path.join(RES, "grafico2_speedup.png")
G3 = os.path.join(RES, "grafico3_fora_dentro.png")

AZUL = RGBColor(0x1F, 0x4E, 0x79)
CINZA = RGBColor(0x40, 0x40, 0x40)

prs = Presentation()
prs.slide_width = Inches(13.333)   # 16:9
prs.slide_height = Inches(7.5)


def add_title_slide():
    s = prs.slides.add_slide(prs.slide_layouts[0])
    s.shapes.title.text = "Ordenação de Produtos Multi-loja"
    s.shapes.title.text_frame.paragraphs[0].font.size = Pt(40)
    s.shapes.title.text_frame.paragraphs[0].font.color.rgb = AZUL
    sub = s.placeholders[1]
    sub.text = "Trabalho P1 — Programação Paralela (SO2)\n" \
               "Programação paralela com processos, threads e container Docker"
    return s


def add_section(title):
    s = prs.slides.add_slide(prs.slide_layouts[1])
    s.shapes.title.text = title
    s.shapes.title.text_frame.paragraphs[0].font.size = Pt(32)
    s.shapes.title.text_frame.paragraphs[0].font.color.rgb = AZUL
    return s


def bullets(slide, items):
    tf = slide.placeholders[1].text_frame
    tf.word_wrap = True
    for i, (texto, nivel) in enumerate(items):
        p = tf.paragraphs[0] if i == 0 else tf.add_paragraph()
        p.text = texto
        p.level = nivel
        p.font.size = Pt(18)
        p.font.color.rgb = CINZA


def add_image_slide(title, path, width_in):
    s = prs.slides.add_slide(prs.slide_layouts[5])
    s.shapes.title.text = title
    s.shapes.title.text_frame.paragraphs[0].font.size = Pt(32)
    s.shapes.title.text_frame.paragraphs[0].font.color.rgb = AZUL
    if os.path.exists(path):
        left = (prs.slide_width - Inches(width_in)) / 2
        s.shapes.add_picture(path, left, Inches(1.6), width=Inches(width_in))
    return s


# ---- Slide 1: título ----
add_title_slide()

# ---- Slide 2: objetivo ----
s = add_section("Objetivo")
bullets(s, [
    ("Consolidar e ordenar produtos de 10+ lojas fictícias por data → valor → id, usando Quicksort.", 0),
    ("Implementar três versões:", 0),
    ("Sequencial", 1),
    ("Paralela com processos + memória compartilhada (fork + mmap)", 1),
    ("Paralela com threads (pthreads)", 1),
    ("Compará-las experimentalmente (speedup) e empacotar em container Docker.", 0),
])

# ---- Slide 3: arquitetura ----
s = add_section("Arquitetura e estratégia de paralelização")
bullets(s, [
    ("Pipeline em 3 fases: geração (entrada) → ordenação (Quicksort) → saída.", 0),
    ("Carga: 10 lojas × 3.000.000 = 30.000.000 produtos.", 0),
    ("Paralelização: divide o vetor em N trechos disjuntos; cada processo/thread gera e ordena o seu trecho; depois uma mesclagem k-vias junta tudo.", 0),
    ("Sincronização: wait() nos processos e pthread_join() nas threads.", 0),
    ("Resultados equivalentes verificados por checksum FNV-1a.", 0),
])

# ---- Slide 4: gráfico 1 ----
add_image_slide("Resultados — Tempo de execução", G1, 8.0)

# ---- Slide 5: gráfico 2 ----
add_image_slide("Resultados — Speedup", G2, 8.0)

# ---- Slide 6: gráfico 3 ----
add_image_slide("Container — Fora × Dentro (limite de CPU)", G3, 10.5)

# ---- Slide 7: conclusão ----
s = add_section("Conclusão")
bullets(s, [
    ("Threads escalam melhor que processos: 1,33x (8 threads) contra 1,04x (8 processos).", 0),
    ("Processos têm pico em 4 workers (1,15x) e caem em 8, pelo custo de fork + mmap.", 0),
    ("A mesclagem sequencial é o gargalo (Lei de Amdahl), limitando o speedup.", 0),
    ("Dentro do container, mais CPUs reduzem o tempo; mais workers com 1 CPU aumentam (contenção).", 0),
    ("Checksum idêntico nas três versões → resultados equivalentes.", 0),
])

saida = os.path.join(DOC, "apresentacao.pptx")
prs.save(saida)
print("Slides gerados em", saida)
