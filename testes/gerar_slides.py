#!/usr/bin/env python3
"""
Gera os slides (PowerPoint) do Trabalho P1 — SO2, com identidade visual rica:
paleta de cores, cartões, badges, diagrama de pipeline, decorações geométricas
e indicador de progresso em todos os slides.

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

# ---- Paleta ----
PRIMARY = RGBColor(0x0B, 0x2E, 0x59)
PRIMARY2 = RGBColor(0x16, 0x47, 0x7E)
ACCENT = RGBColor(0x2E, 0xC4, 0xB6)
ACCENT2 = RGBColor(0xF2, 0x7D, 0x42)
LIGHT = RGBColor(0xF1, 0xF5, 0xFB)
LIGHT2 = RGBColor(0xE3, 0xEC, 0xF8)
DARK = RGBColor(0x1A, 0x1E, 0x2B)
GRAY = RGBColor(0x6B, 0x7B, 0x8A)
LINE = RGBColor(0xD8, 0xDE, 0xE8)
WHITE = RGBColor(0xFF, 0xFF, 0xFF)
FONT = "Calibri"

prs = Presentation()
prs.slide_width = Inches(13.333)
prs.slide_height = Inches(7.5)
SW, SH = prs.slide_width, prs.slide_height


def solid_bg(slide, color):
    slide.background.fill.solid()
    slide.background.fill.fore_color.rgb = color


def shape(slide, kind, l, t, w, h, fill, line=None):
    sh = slide.shapes.add_shape(kind, l, t, w, h)
    if fill is None:
        sh.fill.background()
    else:
        sh.fill.solid()
        sh.fill.fore_color.rgb = fill
    if line is None:
        sh.line.fill.background()
    else:
        sh.line.color.rgb = line
        sh.line.width = Pt(1)
    sh.shadow.inherit = False
    return sh


def rect(slide, l, t, w, h, fill):
    return shape(slide, MSO_SHAPE.RECTANGLE, l, t, w, h, fill)


def card(slide, l, t, w, h, fill=WHITE, border=LINE):
    return shape(slide, MSO_SHAPE.ROUNDED_RECTANGLE, l, t, w, h, fill, border)


def oval(slide, l, t, w, h, fill):
    return shape(slide, MSO_SHAPE.OVAL, l, t, w, h, fill)


def text(slide, l, t, w, h, txt, size, color, bold=False, align=PP_ALIGN.LEFT,
         italic=False, anchor=MSO_ANCHOR.TOP):
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


def decor(slide):
    """Decorações geométricas sutis no fundo."""
    oval(slide, SW - Inches(2.2), -Inches(1.6), Inches(4.6), Inches(4.6), LIGHT)
    oval(slide, SW - Inches(1.4), -Inches(0.8), Inches(3.0), Inches(3.0), LIGHT2)
    shape(slide, MSO_SHAPE.ISOSCELES_TRIANGLE, -Inches(0.4), SH - Inches(1.6),
          Inches(1.6), Inches(1.6), LIGHT)


def dots(slide, total, current):
    cx = SW - Inches(1.4)
    for i in range(total):
        d = Inches(0.14)
        x = cx + Inches(i * 0.34)
        y = Inches(7.16)
        c = ACCENT if i + 1 == current else LINE
        oval(slide, x, y, d, d, c)


def header(slide, titulo, chip, chip_color):
    rect(slide, 0, 0, Inches(0.26), SH, PRIMARY)
    rect(slide, 0, 0, Inches(0.26), Inches(1.6), ACCENT)
    decor(slide)
    cw = Inches(0.4 + 0.115 * len(chip))
    c = card(slide, Inches(0.7), Inches(0.52), cw, Inches(0.46), chip_color)
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
    text(slide, Inches(0.7), Inches(1.12), Inches(12.1), Inches(0.9),
         titulo, 30, PRIMARY, bold=True)
    rect(slide, Inches(0.73), Inches(2.02), Inches(1.7), Pt(3), ACCENT)


def footer(slide, num):
    text(slide, Inches(0.7), Inches(7.18), Inches(8), Inches(0.3),
         "Ordenação de Produtos Multi-loja  ·  P1 — SO2", 10, GRAY)
    dots(slide, 7, num)


def big_card_title(slide, l, t, w, cor, label):
    h = Inches(0.5)
    rect(slide, l, t, w, h, cor)
    text(slide, l + Inches(0.15), t, w - Inches(0.3), h, label, 14, WHITE,
         bold=True, anchor=MSO_ANCHOR.MIDDLE)


# ============ SLIDE 1 — CAPA ============
s = prs.slides.add_slide(prs.slide_layouts[6])
solid_bg(s, PRIMARY)
rect(s, 0, 0, Inches(0.4), SH, ACCENT)
oval(s, SW - Inches(3.6), -Inches(1.7), Inches(6.0), Inches(6.0), PRIMARY2)
oval(s, SW - Inches(3.0), -Inches(1.1), Inches(4.8), Inches(4.8), PRIMARY)
oval(s, -Inches(0.9), SH - Inches(2.4), Inches(3.2), Inches(3.2), PRIMARY2)
rect(s, 0, Inches(6.35), SW, Inches(0.14), ACCENT)
text(s, Inches(1.3), Inches(2.15), Inches(10.6), Inches(1.6),
     "Ordenação de Produtos Multi-loja", 46, WHITE, bold=True)
text(s, Inches(1.3), Inches(3.7), Inches(10.6), Inches(0.8),
     "Trabalho P1 — Programação Paralela (SO2)", 24, ACCENT, bold=True)
text(s, Inches(1.3), Inches(4.55), Inches(10.6), Inches(0.6),
     "Programação paralela com processos, threads e container Docker", 16, RGBColor(0xC7, 0xD3, 0xE4))
text(s, Inches(1.3), Inches(6.6), Inches(10.6), Inches(0.4),
     "Sistemas Operacionais II  ·  2026", 12, RGBColor(0x9F, 0xB3, 0xCB))

# ============ SLIDE 2 — OBJETIVO (cartões) ============
s = prs.slides.add_slide(prs.slide_layouts[6])
solid_bg(s, WHITE)
header(s, "Objetivo", "Contexto", PRIMARY)
text(s, Inches(0.7), Inches(2.35), Inches(12.1), Inches(0.7),
     "Consolidar e ordenar produtos de 10+ lojas fictícias por data → valor → id, "
     "usando Quicksort, e comparar três versões da aplicação:", 18, DARK)

vers = [
    ("SEQUENCIAL", "Algoritmo Quicksort puro\nlinha de base para o speedup", PRIMARY2),
    ("PROCESSOS", "fork + mmap\nmemória compartilhada entre processos", ACCENT2),
    ("THREADS", "pthreads\nmemória compartilhada por padrão (heap)", ACCENT),
]
cw, ch = Inches(3.85), Inches(2.6)
for i, (nome, desc, cor) in enumerate(vers):
    x = Inches(0.7) + Inches(i * 4.15)
    y = Inches(3.35)
    card(s, x, y, cw, ch, LIGHT)
    big_card_title(s, x, y, cw, cor, nome)
    text(s, x + Inches(0.25), y + Inches(0.75), cw - Inches(0.5), ch - Inches(0.9),
         desc, 15, DARK)
footer(s, 2)

# ============ SLIDE 3 — ARQUITETURA (pipeline) ============
s = prs.slides.add_slide(prs.slide_layouts[6])
solid_bg(s, WHITE)
header(s, "Arquitetura e paralelização", "Abordagem", PRIMARY)

fases = [("1", "Geração", "cada loja gera seus\nprodutos (splitmix64)"),
         ("2", "Ordenação", "Quicksort em cada\ntrecho do vetor"),
         ("3", "Saída", "mesclagem k-vias\ndos trechos ordenados")]
for i, (num, nome, desc) in enumerate(fases):
    x = Inches(0.7) + Inches(i * 4.15)
    y = Inches(2.45)
    card(s, x, y, Inches(3.7), Inches(1.9), LIGHT)
    b = oval(s, x + Inches(0.2), y + Inches(0.2), Inches(0.55), Inches(0.55), ACCENT)
    tf = b.text_frame
    tf.paragraphs[0].text = num
    tf.paragraphs[0].font.size = Pt(16)
    tf.paragraphs[0].font.bold = True
    tf.paragraphs[0].font.color.rgb = WHITE
    tf.paragraphs[0].alignment = PP_ALIGN.CENTER
    tf.vertical_anchor = MSO_ANCHOR.MIDDLE
    text(s, x + Inches(0.9), y + Inches(0.15), Inches(2.6), Inches(0.5),
         nome, 18, PRIMARY, bold=True)
    text(s, x + Inches(0.25), y + Inches(0.85), Inches(3.2), Inches(0.95),
         desc, 13, GRAY)
    if i < 2:
        text(s, x + Inches(3.72), y + Inches(0.7), Inches(0.42), Inches(0.5),
             "→", 28, ACCENT, bold=True)

text(s, Inches(0.7), Inches(4.7), Inches(12.1), Inches(0.6),
     "Carga de trabalho: 10 lojas × 3.000.000 = 30.000.000 produtos", 16, DARK, bold=True)
text(s, Inches(0.7), Inches(5.35), Inches(12.1), Inches(0.6),
     "O vetor é dividido em N trechos disjuntos; cada processo/thread gera e ordena o seu "
     "trecho, e a sincronização é feita com wait() (processos) ou pthread_join() (threads).", 14, GRAY)
text(s, Inches(0.7), Inches(6.0), Inches(12.1), Inches(0.5),
     "Equivalência de resultados verificada por checksum FNV-1a.", 14, GRAY, italic=True)
footer(s, 3)

# ============ SLIDES 4–6 — RESULTADOS (gráfico em cartão) ============
def result_slide(titulo, path, width_in, num):
    s = prs.slides.add_slide(prs.slide_layouts[6])
    solid_bg(s, WHITE)
    header(s, titulo, "Resultados", ACCENT2)
    left = (SW - Inches(width_in)) / 2
    card(s, left - Inches(0.2), Inches(2.4), Inches(width_in) + Inches(0.4), Inches(4.3), LIGHT)
    if os.path.exists(path):
        s.shapes.add_picture(path, left, Inches(2.55), width=Inches(width_in))
    footer(s, num)
    return s

result_slide("Tempo de execução por versão", G1, 7.6, 4)
result_slide("Speedup (t_seq / t_par)", G2, 7.6, 5)
result_slide("Container: fora × dentro (limite de CPU)", G3, 10.4, 6)

# ============ SLIDE 7 — CONCLUSÃO (cartões de destaque) ============
s = prs.slides.add_slide(prs.slide_layouts[6])
solid_bg(s, WHITE)
header(s, "Conclusão", "Fechamento", ACCENT)

stats = [
    ("1,33x", "speedup com 8 threads\n(threads escalam melhor)", ACCENT),
    ("1,04x", "speedup com 8 processos\n(custo de fork + mmap)", ACCENT2),
    ("Amdahl", "a mesclagem sequencial\né o gargalo do speedup", PRIMARY2),
    ("CPU 1→4", "no container, mais CPUs\nreduzem o tempo total", ACCENT),
    ("checksum", "idêntico nas três versões\nresultados equivalentes", PRIMARY2),
]
for i, (num, desc, cor) in enumerate(stats):
    col = i % 3
    row = i // 3
    x = Inches(0.7) + Inches(col * 4.15)
    y = Inches(2.5) + Inches(row * 2.35)
    card(s, x, y, Inches(3.85), Inches(2.0), LIGHT)
    text(s, x + Inches(0.25), y + Inches(0.18), Inches(3.35), Inches(0.7),
         num, 26, cor, bold=True)
    text(s, x + Inches(0.25), y + Inches(1.0), Inches(3.35), Inches(0.85),
         desc, 12, GRAY)
footer(s, 7)

saida = os.path.join(DOC, "apresentacao.pptx")
prs.save(saida)
print("Slides gerados em", saida)
