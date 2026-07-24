#ifndef FUNCOES_BINARIAS_PARA_BUFFER_H
#define FUNCOES_BINARIAS_PARA_BUFFER_H

#include <stdint.h>

uint64_t ler_uint64_t(uint8_t *buffer, int indiceBuffer);
uint32_t ler_uint32_t(uint8_t *buffer, int indiceBuffer);
uint16_t ler_uint16_t(uint8_t *buffer, int indiceBuffer);

#endif /* FUNCOES_BINARIAS_PARA_BUFFER_H*/
