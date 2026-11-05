#ifndef GERADOR_H
#define GERADOR_H

#include <stdint.h>
#include "modelo.h"


Produto *gerar_produtos(uint32_t num_lojas, uint64_t por_loja, uint64_t seed);


void gerar_faixa(Produto *v, uint64_t inicio, uint64_t fim, uint64_t seed, uint64_t por_loja);

#endif
