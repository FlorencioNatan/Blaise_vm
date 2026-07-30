#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../../bvm/src/bvm.h"

bool ler_uma_linha = false;
bool terminou_debug = false;
char ultimo_comando[256] = "";

void processar_comandos_debug(bvm *vm) {
	char comando[256] = "";
	scanf("%[^\n]", comando);
	scanf("%*c");

	if (strcmp(comando, "") == 0 && strcmp(ultimo_comando, "") != 0) {
		strcpy(comando, ultimo_comando); // Não preicsa copiar o \0, porque a string foi inicialiada com ""
	}

	strcpy(ultimo_comando, comando);

	if (strcmp(comando, "stepi") == 0 || strcmp(comando, "si") == 0) {
		ler_uma_linha = true;
		return;
	}

	if (strcmp(comando, "nexti") == 0 || strcmp(comando, "ni") == 0) {
		ler_uma_linha = true;
		return;
	}

	if (strcmp(comando, "exit") == 0 || strcmp(comando, "quit") == 0) {
		terminou_debug = true;
		return;
	}

	printf("Comando desconhecido!\n");
}

void executar_maquina_virtual(bvm *vm) {
    if (vm->tam_programa == 0) {
		printf("Não tem nenhum programa carregado!!!\n");
		return;
	}
    if (vm->halt == 1) {
		printf("O prorama já finalizou!!!\n");
		return;
	}
    if (vm->pc > vm->tam_programa) {
		printf("O prorama já finalizou!!!\n");
		return;
	}
	if (!ler_uma_linha) {
		return;
	}

	int resultadoExecucao = processar_instrucoes(vm);

	switch (resultadoExecucao) {
			break;
		case EXEC_ERRO_INSTRUCAO_INVALIDA:
			vm->halt = 1;
			printf("Instrução desconhecida: %ld\n", vm->pc-1);
			break;
		case EXEC_ERRO_STACK_UNDERFLOW:
			vm->halt = 1;
			printf("Stack underflow na instrução: %ld\n", vm->pc-1);
			break;
		case EXEC_ERRO_STACK_OVERFLOW:
			vm->halt = 1;
			printf("Stack overflow na instrução: %ld\n", vm->pc-1);
			break;
		case EXEC_ERRO_DIVISAO_POR_ZERO:
			vm->halt = 1;
			printf("Divisão por zero na instrução: %ld\n", vm->pc-1);
			break;
		case EXEC_ERRO_ACESSO_INVALIDO_A_MEMORIA:
			vm->halt = 1;
			printf("Acesso inválido à memória na instrução: %ld\n", vm->pc-1);
			break;
		case EXEC_ERRO_EXTENSAO_NAO_EXISTE:
			vm->halt = 1;
			printf("A extensão não existe: %ld\n", vm->pc-1);
			break;
		case EXEC_ERRO_TAMANHO_MEMORIA_PEQUENO_PARA_EXTENSAO:
			vm->halt = 1;
			printf("Tamanho de memória muito pequeno para a extensão: %ld\n", vm->pc-1);
			break;
	}

	ler_uma_linha = false;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("É necessário especificar o arquivo do programa!\n");
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

	while (!terminou_debug) {
		processar_comandos_debug(&vm);
		executar_maquina_virtual(&vm);
	}

	free(vm.variaveis);
	free(vm.sourceMaps);
}
