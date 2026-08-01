#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../../bvm/src/bvm.h"
#include "../../misc/auxiliar/funcoes_binarias_para_buffer.h"

bool ler_uma_linha_assembly = false;
bool ler_uma_linha_codigo = false;
bool terminou_debug = false;
uint32_t ultima_linha_codigo = 0;
char ultimo_comando[256] = "";
char** codigo_fonte;
uint32_t quantidade_linhas_codigo_fonte = 0;

void ler_codigo_fonte(char* nomeArquivo) {
    FILE *arquivoBbvm = fopen(nomeArquivo, "r");
    fseek(arquivoBbvm, 0, SEEK_END);
    int tamanhoArquivoBytes = ftell(arquivoBbvm);
    bool modoDebug = false;

    if (tamanhoArquivoBytes > CINCO_MB) {
        printf("O tamanho máximo do arquivo de assembly suportado é 5MB.");
		return;
    }
    fseek(arquivoBbvm, 0, SEEK_SET);

    uint8_t buffer[tamanhoArquivoBytes + 1];
    fread(buffer, sizeof(buffer), 1, arquivoBbvm);

	if (buffer[3] != VERSAO_DEBUG) {
		return;
	}

    uint32_t tamanhoPrograma = ler_uint32_t(buffer, POSICAO_TAMANHO_PROGRAMA);

    uint32_t tamanhoArquivo = ler_uint32_t(buffer, POSICAO_TAMANHO_ARQUIVO);
	uint32_t tamanhoData = ler_uint32_t(buffer, POSICAO_TAMANHO_DATA);
	uint32_t tamanhoDebug = ler_uint32_t(buffer, POSICAO_TAMANHO_DEBUG);

    uint32_t inicioSecaoSource = TAMANHO_CABECALHO_BINARIO_DEBUG + tamanhoPrograma + tamanhoData + tamanhoDebug;

	for(uint32_t i = 0; i <= strlen((char*) &buffer[inicioSecaoSource]); i++) {
		if (((char*) &buffer[inicioSecaoSource])[i] == '\n') {
			quantidade_linhas_codigo_fonte++;
		}
	}
	quantidade_linhas_codigo_fonte++;

	codigo_fonte = malloc(sizeof(char**) * quantidade_linhas_codigo_fonte);

	char* line = (char*) &buffer[inicioSecaoSource];
	char* new_line = strchr(line, '\n');

	uint32_t i = 0;
	while (new_line != NULL) {
		*new_line = '\0';
		codigo_fonte[i] = malloc(sizeof(char*) * strlen(line) + 1);
		strcpy(codigo_fonte[i], line);
		codigo_fonte[i++][strlen(line)] = '\0';
		// *new_line = '\n';
		line = new_line + 1;
		new_line = strchr(line, '\n');
	}

	codigo_fonte[i] = malloc(sizeof(char*) * strlen(line) + 1);
	strcpy(codigo_fonte[i], line);
	codigo_fonte[i++][strlen(line)] = '\0';
}

int obter_linha_de_codigo_atual_da_vm(bvm* vm) {
	for (uint32_t i = 0; i < vm->tam_sourceMap; i++) {
		sourceMap_t sm = vm->sourceMaps[i];
		if (sm.posInicialAssembly <= vm->pc && sm.posFinalAssembly >= vm->pc) {
			return sm.linhaSource;
		}
	}

	return 1;
}

void processar_comandos_debug(bvm *vm) {
	if (ler_uma_linha_codigo) {
		return;
	}

	char comando[256] = "";
	scanf("%[^\n]", comando);
	scanf("%*c");

	if (strcmp(comando, "") == 0 && strcmp(ultimo_comando, "") != 0) {
		strcpy(comando, ultimo_comando); // Não preicsa copiar o \0, porque a string foi inicialiada com ""
	}

	strcpy(ultimo_comando, comando);

	if (strcmp(comando, "stepi") == 0 || strcmp(comando, "si") == 0) {
		ler_uma_linha_assembly = true;
		return;
	}

	if (strcmp(comando, "nexti") == 0 || strcmp(comando, "ni") == 0) {
		ler_uma_linha_assembly = true;
		return;
	}

	if (strcmp(comando, "next") == 0 || strcmp(comando, "n") == 0) {
		ler_uma_linha_codigo = true;
		return;
	}

	if (strcmp(comando, "list") == 0 || strcmp(comando, "l") == 0) {
		int32_t linha_central = obter_linha_de_codigo_atual_da_vm(vm),
			linha_inicial = linha_central - 5 > 1 ? linha_central - 5 : 1,
			linha_final = linha_central + 5 < quantidade_linhas_codigo_fonte -1 ? linha_central + 5 : quantidade_linhas_codigo_fonte -1;

		for (int32_t i = linha_inicial; i <= linha_final; i++) {
			if (i == linha_central) {
				printf("-> ");
			}
			printf("%s\n", codigo_fonte[i]);
		}

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
		ler_uma_linha_codigo = false;
		printf("Não tem nenhum programa carregado!!!\n");
		return;
	}
    if (vm->halt == 1) {
		ler_uma_linha_codigo = false;
		printf("O prorama já finalizou!!!\n");
		return;
	}
    if (vm->pc > vm->tam_programa) {
		printf("O prorama já finalizou!!!\n");
		ler_uma_linha_codigo = false;
		return;
	}
	if (!ler_uma_linha_assembly && !ler_uma_linha_codigo) {
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

	ler_uma_linha_assembly = false;

	uint32_t linha_atual = obter_linha_de_codigo_atual_da_vm(vm);
	if (linha_atual != ultima_linha_codigo) {
		ler_uma_linha_codigo = false;
		ultima_linha_codigo = linha_atual;
	}
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
    ler_codigo_fonte(argv[1]);

	while (!terminou_debug) {
		processar_comandos_debug(&vm);
		executar_maquina_virtual(&vm);
	}

	free(vm.variaveis);
	free(vm.sourceMaps);
}
