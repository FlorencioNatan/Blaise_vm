#include "funcoes_binarias_para_buffer.h"

uint64_t ler_uint64_t(uint8_t *buffer, int indiceBuffer) {
    uint64_t tamanhoPrograma = 0;
    for (int i = 64; i >= 8;i -= 8) {
        tamanhoPrograma |= ((uint64_t) buffer[indiceBuffer++]) << (64 - i);
    }
    return tamanhoPrograma;
}

uint32_t ler_uint32_t(uint8_t *buffer, int indiceBuffer) {
    uint32_t tamanhoPrograma = 0;
    for (int i = 32; i >= 8;i -= 8) {
        tamanhoPrograma |= ((uint32_t) buffer[indiceBuffer++]) << (32 - i);
    }
    return tamanhoPrograma;
}

uint16_t ler_uint16_t(uint8_t *buffer, int indiceBuffer) {
    uint16_t tamanhoPrograma = 0;
    for (int i = 16; i >= 8;i -= 8) {
        tamanhoPrograma |= ((uint16_t) buffer[indiceBuffer++]) << (16 - i);
    }
    return tamanhoPrograma;
}
