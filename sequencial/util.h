#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <stdint.h>
#include "modelo.h"


double agora_segundos(void);


uint64_t checksum_produtos(const Produto *v, size_t n);


int verificar_ordenado(const Produto *v, size_t n);

void imprimir_produto(const Produto *p);


void formatar_data(uint64_t ts, char *buf, size_t n);


void mesclar_k_vias(Produto *dst, const Produto *src, int k, const uint64_t *inicios);

#endif
