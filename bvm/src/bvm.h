#ifndef BVM_H
#define BVM_H

#include <stdint.h>

#define CINCO_MB 5000000
#define POSICAO_TAMANHO_ARQUIVO 4
#define POSICAO_TAMANHO_PROGRAMA 8
#define POSICAO_TAMANHO_DATA 12
#define TAMANHO_CABECALHO_BINARIO 12
#define TAMANHO_CABECALHO_BINARIO_DEBUG 20

#define VERSAO_ATUAL 1
#define VERSAO_DEBUG 0xFF

#define MAX_TAM_PILHA 64
#define MAX_TAM_MEMORIA 5000000 // 5MB
#define TIPO_DADO_64_BITS 8
#define TIPO_DADO_32_BITS 4
#define TIPO_DADO_16_BITS 2
#define TIPO_DADO_8_BITS 1

#define EXEC_SUCESSO 0
#define EXEC_ERRO_INSTRUCAO_INVALIDA 1
#define EXEC_ERRO_STACK_UNDERFLOW 2
#define EXEC_ERRO_STACK_OVERFLOW 3
#define EXEC_ERRO_DIVISAO_POR_ZERO 4
#define EXEC_ERRO_ACESSO_INVALIDO_A_MEMORIA 5
#define EXEC_ERRO_EXTENSAO_NAO_EXISTE 6
#define EXEC_ERRO_TAMANHO_MEMORIA_PEQUENO_PARA_EXTENSAO 7

typedef struct bvm {
    uint64_t pc;

    uint64_t pilha[MAX_TAM_PILHA];
    uint64_t tam_pilha;

    uint8_t *programa;
    uint64_t tam_programa;

    uint8_t memoria[MAX_TAM_MEMORIA];
    uint8_t halt;
} bvm;

void ler_programa_do_arquivo_binario(char* nomeArquivoBbvm, bvm *vm);
int processar_instrucoes(bvm *vm);
void escreve_uint64_t_na_memoria(bvm *vm, uint64_t endereco, uint64_t valor);
void escreve_uint32_t_na_memoria(bvm *vm, uint64_t endereco, uint32_t valor);
void escreve_uint16_t_na_memoria(bvm *vm, uint64_t endereco, uint16_t valor);
void escreve_double_na_memoria(bvm *vm, uint64_t endereco, double valor);

int le_int_da_memoria(bvm *vm, uint64_t endereco);
char* le_string_da_memoria(bvm *vm, uint64_t endereco, int tamanho);
double le_double_da_memoria(bvm *vm, uint64_t endereco);

#endif /* BVM_H */
