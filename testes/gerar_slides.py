#!/usr/bin/env python3
"""
Gera os slides (PowerPoint) de apresentação do Trabalho P1 — SO2,
com identidade visual: paleta de cores, faixas e elementos gráficos.

Requer: python-pptx (pip install python-pptx).

Entrada: os gráficos em testes/resultados/*.png
Saída:   documentacao/apresentacao.pptx
"""

import os
from pptx import Presentation
from pptx.util import Inches, Pt
from pptx.dml.color import RGBColor
from pptx.enum.text import PP_ALIGN, MSO_ANCHOR
from pptx.enum.shapes import MSO_SHAPE

BASE = os.path.dirname(os.path.abspath(__file__))
RAIZ = os.path.dirname(BASE)
RES = os.path.join(BASE, "resultados")
DOC = os.path.join(RAIZ, "documentacao")
os.makedirs(DOC, exist_ok=True)

G1 = os.path.join(RES, "grafico1_tempo.png")
G2 = os.path.join(RES, "grafico2_speedup.png")
G3 = os.path.join(RES, "grafico3_fora_dentro.png")

# ---- Paleta / identidade visual ----
PRIMARY = RGBColor(0x0B, 0x2E, 0x59)   # azul-marinho (principal)
PRIMARY2 = RGBColor(0x16, 0x47, 0x7E)  # azul médio
ACCENT = RGBColor(0x2E, 0xC4, 0xB6)    # teal (destaque)
ACCENT2 = RGBColor(0xF2, 0x7D, 0x42)   # laranja (realce)
LIGHT = RGBColor(0xF2, 0xF5, 0xFA)     # fundo claro
DARK = RGBColor(0x1A, 0x1E, 0x2B)      # texto escuro
GRAY = RGBColor(0x6B, 0x7B, 0x8A)      # cinza
WHITE = RGBColor(0xFF, 0xFF, 0xFF)

FONT = "Calibri"

prs = Presentation()
prs.slide_width = Inches(13.333)
prs.slide_height = Inches(7.5)
SW, SH = prs.slide_width, prs.slide_height


def solid_bg(slide, color):
    slide.background.fill.solid()
    slide.background.fill.fore_color.rgb = color


def rect(slide, l, t, w, h, color):
    sh = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, l, t, w, h)
    sh.fill.solid()
    sh.fill.fore_color.rgb = color
    sh.line.fill.background()
    sh.shadow.inherit = False
    return sh


def oval(slide, l, t, w, h, color):
    sh = slide.shapes.add_shape(MSO_SHAPE.OVAL, l, t, w, h)
    sh.fill.solid()
    sh.fill.fore_color.rgb = color
    sh.line.fill.background()
    sh.shadow.inherit = False
    return sh


def text(slide, l, t, w, h, txt, size, color, bold=False,
         align=PP_ALIGN.LEFT, italic=False, anchor=MSO_ANCHOR.TOP):
    box = slide.shapes.add_textbox(l, t, w, h)
    tf = box.text_frame
    tf.word_wrap = True
    tf.vertical_anchor = anchor
    p = tf.paragraphs[0]
    p.text = txt
    p.font.name = FONT
    p.font.size = Pt(size)
    p.font.color.rgb = color
    p.font.bold = bold
    p.font.italic = italic
    p.alignment = align
    return box


def footer(slide, num):
    rect(slide, Inches(0.55), Inches(7.05), SW - Inches(1.1), Pt(1.1), RGBColor(0xD8, 0xDE, 0xE8))
    text(slide, Inches(0.55), Inches(7.12), Inches(8), Inches(0.3),
         "Ordenação de Produtos Multi-loja  ·  P1 — SO2", 10, GRAY)
    text(slide, SW - Inches(1.7), Inches(7.12), Inches(1.15), Inches(0.3),
         f"{num:02d}", 10, GRAY, align=PP_ALIGN.RIGHT)


def header(slide, titulo, chip, chip_color):
    # faixa lateral de identidade
    rect(slide, 0, 0, Inches(0.22), SH, PRIMARY)
    # chip (etiqueta da seção)
    chip_w = Inches(0.22 + 0.5 + len(chip) * 0.09)
    c = rect(slide, Inches(0.62), Inches(0.5), chip_w, Inches(0.42), chip_color)
    tf = c.text_frame
    tf.word_wrap = False
    p = tf.paragraphs[0]
    p.text = chip.upper()
    p.font.name = FONT
    p.font.size = Pt(11)
    p.font.bold = True
    p.font.color.rgb = WHITE
    p.alignment = PP_ALIGN.CENTER
    tf.vertical_anchor = MSO_ANCHOR.MIDDLE
    # título
    text(slide, Inches(0.62), Inches(1.05), Inches(12.2), Inches(0.9),
         titulo, 30, PRIMARY, bold=True)
    # sublinhado de destaque
    rect(slide, Inches(0.65), Inches(1.95), Inches(1.6), Pt(3), ACCENT)


def bullets(slide, items, top=Inches(2.35)):
    tf = slide.shapes.add_textbox(Inches(0.62), top, Inches(12.1), Inches(4.4)).text_frame
    tf.word_wrap = True
    for i, (txt, nivel) in enumerate(items):
        p = tf.paragraphs[0] if i == 0 else tf.add_paragraph()
        p.text = ("•  " if nivel == 0 else "–  ") + txt
        p.level = nivel
        p.font.name = FONT
        p.font.size = Pt(17 if nivel == 0 else 15)
        p.font.color.rgb = DARK if nivel == 0 else GRAY
        p.space_after = Pt(10 if nivel == 0 else 4)
    return tf


def image_slide(titulo, chip, path, width_in, num):
    s = prs.slides.add_slide(prs.slide_layouts[6])
    solid_bg(s, WHITE)
    header(s, titulo, chip, ACCENT2)
    if os.path.exists(path):
        left = (SW - Inches(width_in)) / 2
        # cartão atrás da imagem
        rect(s, left - Inches(0.15), Inches(2.3), Inches(width_in) + Inches(0.3),
             Inches(4.6), LIGHT)
        s.shapes.add_picture(path, left, Inches(2.45), width=Inches(width_in))
    footer(s, num)
    return s


# ============ SLIDE 1 — CAPA ============
s = prs.slides.add_slide(prs.slide_layouts[6])
solid_bg(s, PRIMARY)
# elementos gráficos decorativos
rect(s, 0, 0, Inches(0.35), SH, ACCENT)
oval(s, SW - Inches(3.4), -Inches(1.4), Inches(5.5), Inches(5.5), PRIMARY2)
oval(s, SW - Inches(2.9), -Inches(0.9), Inches(4.5), Inches(4.5), PRIMARY)
rect(s, 0, Inches(6.4), SW, Inches(0.12), ACCENT)
# textos
text(s, Inches(1.1), Inches(2.1), Inches(11), Inches(1.6),
     "Ordenação de Produtos Multi-loja", 44, WHITE, bold=True)
text(s, Inches(1.1), Inches(3.6), Inches(11), Inches(0.8),
     "Trabalho P1 — Programação Paralela (SO2)", 24, ACCENT, bold=True)
text(s, Inches(1.1), Inches(4.45), Inches(11), Inches(0.6),
     "Programação paralela com processos, threads e container Docker", 16, RGBColor(0xC7, 0xD3, 0xE4))
text(s, Inches(1.1), Inches(6.6), Inches(11), Inches(0.4),
     "Universidade  ·  Sistemas Operacionais II  ·  2026", 12, RGBColor(0x9F, 0xB3, 0xCB))

# ============ SLIDE 2 — OBJETIVO ============
s = prs.slides.add_slide(prs.slide_layouts[6])
solid_bg(s, WHITE)
header(s, "Objetivo", "Contexto", PRIMARY)
bullets(s, [
    ("Consolidar e ordenar produtos de 10+ lojas fictícias por data → valor → id, usando Quicksort.", 0),
    ("Implementar três versões da aplicação:", 0),
    ("Sequencial", 1),
    ("Paralela com processos + memória compartilhada (fork + mmap)", 1),
    ("Paralela com threads (pthreads)", 1),
    ("Compará-las experimentalmente (speedup) e empacotar em container Docker.", 0),
])
footer(s, 2)

# ============ SLIDE 3 — ARQUITETURA ============
s = prs.slides.add_slide(prs.slide_layouts[6])
solid_bg(s, WHITE)
header(s, "Arquitetura e estratégia", "Abordagem", PRIMARY)
bullets(s, [
    ("Pipeline em 3 fases: geração (entrada) → ordenação (Quicksort) → saída.", 0),
    ("Carga de trabalho: 10 lojas × 3.000.000 = 30.000.000 produtos.", 0),
    ("Paralelização: divide o vetor em N trechos disjuntos; cada processo/thread gera e ordena o seu trecho; depois uma mesclagem k-vias junta tudo.", 0),
    ("Sincronização: wait() nos processos e pthread_join() nas threads.", 0),
    ("Equivalência de resultados verificada por checksum FNV-1a.", 0),
])
footer(s, 3)

# ============ SLIDES 4–6 — RESULTADOS ============
image_slide("Tempo de execução por versão", "Resultados", G1, 8.0, 4)
image_slide("Speedup (t_seq / t_par)", "Resultados", G2, 8.0, 5)
image_slide("Container: fora × dentro (limite de CPU)", "Resultados", G3, 10.6, 6)

# ============ SLIDE 7 — CONCLUSÃO ============
s = prs.slides.add_slide(prs.slide_layouts[6])
solid_bg(s, WHITE)
header(s, "Conclusão", "Fechamento", ACCENT)
bullets(s, [
    ("Threads escalam melhor que processos: 1,33x (8 threads) contra 1,04x (8 processos).", 0),
    ("Processos têm pico em 4 workers (1,15x) e caem em 8, pelo custo de fork + mmap.", 0),
    ("A mesclagem sequencial é o gargalo (Lei de Amdahl), limitando o speedup.", 0),
    ("No container, mais CPUs reduzem o tempo; mais workers com 1 CPU aumentam (contenção).", 0),
    ("Checksum idêntico nas três versões → resultados equivalentes.", 0),
])
footer(s, 7)

saida = os.path.join(DOC, "apresentacao.pptx")
prs.save(saida)
print("Slides gerados em", saida)
