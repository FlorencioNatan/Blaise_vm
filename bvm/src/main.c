#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bvm.h"
#include "instrucoes.h"
#include "../../misc/auxiliar/funcoes_binarias_para_buffer.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("É necessário especificar o arquivo de assembly!\n");
        return 0;
    }

    bvm vm = {
        .pc = 0,
        .pilha = {0},
        .tam_pilha = 0,
        .tam_programa = 0,
        .memoria = {0},
        .halt = 0
    };

    ler_programa_do_arquivo_binario(argv[1], &vm);

    while (!vm.halt && vm.tam_programa != 0 && vm.pc <= vm.tam_programa) {
        int resultadoExecucao = processar_instrucoes(&vm);

        switch (resultadoExecucao) {
                break;
            case EXEC_ERRO_INSTRUCAO_INVALIDA:
                vm.halt = 1;
                printf("Instrução desconhecida: %ld\n", vm.pc-1);
                break;
            case EXEC_ERRO_STACK_UNDERFLOW:
                vm.halt = 1;
                printf("Stack underflow na instrução: %ld\n", vm.pc-1);
                break;
            case EXEC_ERRO_STACK_OVERFLOW:
                vm.halt = 1;
                printf("Stack overflow na instrução: %ld\n", vm.pc-1);
                break;
            case EXEC_ERRO_DIVISAO_POR_ZERO:
                vm.halt = 1;
                printf("Divisão por zero na instrução: %ld\n", vm.pc-1);
                break;
            case EXEC_ERRO_ACESSO_INVALIDO_A_MEMORIA:
                vm.halt = 1;
                printf("Acesso inválido à memória na instrução: %ld\n", vm.pc-1);
                break;
            case EXEC_ERRO_EXTENSAO_NAO_EXISTE:
                vm.halt = 1;
                printf("A extensão não existe: %ld\n", vm.pc-1);
                break;
            case EXEC_ERRO_TAMANHO_MEMORIA_PEQUENO_PARA_EXTENSAO:
                vm.halt = 1;
                printf("Tamanho de memória muito pequeno para a extensão: %ld\n", vm.pc-1);
                break;
        }
    }
}
