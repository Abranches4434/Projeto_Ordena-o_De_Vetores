#ifndef MODELO_H
#define MODELO_H

#include <stdint.h>




#define NUM_LOJAS_PADRAO 10


#define PRODUTOS_POR_LOJA_PADRAO 3000000

// Um produto tem: id, loja de origem, preço, data e quantidade em estoque.
typedef struct {
    uint64_t id;
    uint32_t loja_id;
    double   preco;
    uint64_t data;
    uint32_t quantidade;
} Produto;


// Ordem da ordenação: primeiro por data, depois por preço e, por fim, pelo id.
static inline int compara_produto(const Produto *a, const Produto *b) {
    if (a->data != b->data)   return (a->data < b->data)   ? -1 : 1;
    if (a->preco != b->preco) return (a->preco < b->preco) ? -1 : 1;
    if (a->id != b->id)       return (a->id < b->id)       ? -1 : 1;
    return 0;
}

#endif
