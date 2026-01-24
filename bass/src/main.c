#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "instrucoes.h"
#include "tabelaJump.h"

#define CINCO_MB 5000000

#define ESTADO_PROCESSAMANETO_INVALIDO 0
#define ESTADO_PROCESSAMANETO_CODE     1
#define ESTADO_PROCESSAMANETO_DATA     2
#define TAMANHO_CABECALHO_BINARIO 12
#define TAMANHO_OFFSET_DATA 17

int estadoProcessamento = ESTADO_PROCESSAMANETO_INVALIDO;
int tamanhoCode = 0;
int tamanhoArquivo = 0;
uint8_t* conteudo;

void calcular_tamanho_instrucao(
    int *totalJumps,
    char **token
) {
    if (estadoProcessamento != ESTADO_PROCESSAMANETO_CODE) {
            return;
    }

    if ((*token)[strlen((*token))-1] == ':') {
        (*totalJumps)++;
        (*token) = strtok (NULL," \t\n,");
        return;
    }
    registroInstrucao instrucao = lookup_instrucao((*token));
    tamanhoArquivo += instrucao.tamanho;
    tamanhoCode += instrucao.tamanho;

    if (instrucao.tem_parametro) {
        strtok(NULL," \t\n,");
    }

    (*token) = strtok(NULL," \t\n,");
}

void calcular_tamanho_data(
    char **token
) {
    if (estadoProcessamento != ESTADO_PROCESSAMANETO_DATA) {
        return;
    }

    (*token) = strtok (NULL," \t\n,");
    char tipo[5];
    strcpy(tipo, (*token));

    if (strcmp(tipo, "strg") != 0 && strcmp(tipo, "strc") != 0) {
        (*token) = strtok (NULL," \t\n,");
        int64_t quantidade = strtoul((*token), NULL, 0);

        int64_t tamanhoTipo = 1;
        if (strcmp(tipo, "word") == 0) {
            tamanhoTipo = 8;
        } else if (strcmp(tipo, "half") == 0) {
            tamanhoTipo = 4;
        } else if (strcmp(tipo, "quar") == 0) {
            tamanhoTipo = 2;
        }

        tamanhoArquivo += (quantidade * tamanhoTipo) + TAMANHO_OFFSET_DATA;

        for (int i = 0; i < quantidade; i++) {
            (*token) = strtok (NULL," \t\n,");
        }
        (*token) = strtok (NULL," \t\n,");
        return;
    }

    if (strcmp(tipo, "strc") == 0) {
        tamanhoArquivo += 1;
    }

    (*token) = strtok (NULL,"\"");
    tamanhoArquivo += strlen((*token)) + TAMANHO_OFFSET_DATA;
    (*token) = strtok (NULL," \t\n,");
}

void calcular_tamanho_arquivo(
    char *assembly,
    int *totalJumps
) {
    char *token;
    char *assemblyCopy = malloc(strlen(assembly)+1);
    strcpy(assemblyCopy, assembly);
    token = strtok(assemblyCopy," \t\n,");
    do {
        if (strcmp(token, ".code") == 0) {
            estadoProcessamento = ESTADO_PROCESSAMANETO_CODE;
            token = strtok(NULL," \t\n,");
            continue;
        }

        if (strcmp(token, ".data") == 0) {
            estadoProcessamento = ESTADO_PROCESSAMANETO_DATA;
            token = strtok(NULL," \t\n,");
            continue;
        }

        calcular_tamanho_instrucao(totalJumps, &token);
        calcular_tamanho_data(&token);
    } while (token != NULL);
    estadoProcessamento = ESTADO_PROCESSAMANETO_INVALIDO;
    free(assemblyCopy);
}

int checa_se_string_e_numero(char *str) {
    return (str[0] >= '0' && str[0] <= '9') || str[0] == '-' || str[0] == '+';
}

void insere_uint64_t_no_conteudo(
    uint8_t* conteudo,
    int *indiceConteudo,
    uint64_t valor
) {
    uint8_t *byteParametro = (uint8_t*)&valor;
    for (uint64_t i = 0; i < sizeof(uint64_t); i++) {
        conteudo[(*indiceConteudo)++] = byteParametro[i];
    }
}

void insere_uint32_t_no_conteudo(
    uint8_t* conteudo,
    int *indiceConteudo,
    uint64_t valor
) {
    uint8_t *byteParametro = (uint8_t*)&valor;
    for (uint32_t i = 0; i < sizeof(uint32_t); i++) {
        conteudo[(*indiceConteudo)++] = byteParametro[i];
    }
}

void insere_uint16_t_no_conteudo(
    uint8_t* conteudo,
    int *indiceConteudo,
    uint64_t valor
) {
    uint8_t *byteParametro = (uint8_t*)&valor;
    for (uint16_t i = 0; i < sizeof(uint16_t); i++) {
        conteudo[(*indiceConteudo)++] = byteParametro[i];
    }
}

void insere_push_parametro_e_instrucao(
    uint8_t* conteudo,
    int *indiceConteudo,
    registroInstrucao instrucao,
    uint64_t parametro
) {
    conteudo[(*indiceConteudo)++] = INST_PUSH;
    insere_uint64_t_no_conteudo(conteudo, indiceConteudo, parametro);
    conteudo[(*indiceConteudo)++] = instrucao.codigo;
}

void processar_token_code(
    char *token,
    jump *tabelaJumps,
    int totalJumps,
    uint8_t* conteudo,
    int *indiceConteudo
){
    if (estadoProcessamento != ESTADO_PROCESSAMANETO_CODE) {
        return;
    }

    if (token[strlen(token)-1] == ':') {
        return;
    }

    registroInstrucao instrucao = lookup_instrucao(token);

    if (!instrucao.tem_parametro) {
        conteudo[(*indiceConteudo)++] = instrucao.codigo;
        return;
    }

    token = strtok(NULL, " \t\n,");
    if (token == NULL) {
        conteudo[(*indiceConteudo)++] = instrucao.codigo;
        return;
    }

    if (!checa_se_string_e_numero(token)) {
        /**
         * Está trando a instrução ADDU como se fosse um jump
         */
        jump *jumpEscolhido = lookup_jump(tabelaJumps, totalJumps, token);
        if (jumpEscolhido != NULL) {
            insere_push_parametro_e_instrucao(
                conteudo,
                indiceConteudo,
                instrucao,
                jumpEscolhido->posicao
            );
        }
        return;
    }

    if (instrucao.codigo == INST_PUSH) {
        conteudo[(*indiceConteudo)++] = instrucao.codigo;

        if (strstr(token, ".") != NULL) {
            double parametro = strtod(token, NULL);
            uint64_t* parametroAsInt = (uint64_t *) &parametro;

            insere_uint64_t_no_conteudo(conteudo, indiceConteudo, *parametroAsInt);
            return;
        }

        if (token[0] == '-') {
            int64_t parametro = strtol(token, NULL, 0);
            insere_uint64_t_no_conteudo(conteudo, indiceConteudo, parametro);
            return;
        }

        uint64_t parametro = strtoul(token, NULL, 0);
        insere_uint64_t_no_conteudo(conteudo, indiceConteudo, parametro);
        return;
    }

    if (instrucao.tem_parametro) {
        uint64_t parametro = strtoul(token, NULL, 0);
        insere_push_parametro_e_instrucao(
            conteudo,
            indiceConteudo,
            instrucao,
            parametro
        );
        return;
    }
}

void processar_token_data(
    char *token,
    uint8_t* conteudo,
    int *indiceConteudo
){
    if (estadoProcessamento != ESTADO_PROCESSAMANETO_DATA) {
        return;
    }

    uint64_t endereco = strtoul(token, NULL, 0);
    insere_uint64_t_no_conteudo(conteudo, indiceConteudo, endereco);

    token = strtok (NULL," \t\n,");
    char tipo[4];
    strcpy(tipo, token);

    if (strcmp(tipo, "strg") != 0 && strcmp(tipo, "strc")) {
        if (strcmp(tipo, "word") == 0) {
            conteudo[(*indiceConteudo)++] = 8;
        } else if (strcmp(tipo, "half") == 0) {
            conteudo[(*indiceConteudo)++] = 4;
        } else if (strcmp(tipo, "quar") == 0) {
            conteudo[(*indiceConteudo)++] = 2;
        } else {
            conteudo[(*indiceConteudo)++] = 1;
        }

        token = strtok (NULL," \t\n,");
        int64_t quantidade = strtoul(token, NULL, 0);
        insere_uint64_t_no_conteudo(conteudo, indiceConteudo, quantidade);

        for (int i = 0; i < quantidade; i++) {
            token = strtok (NULL," \t\n,");

            if (strcmp(tipo, "word") == 0) {
                if (token[0] == '-') {
                    int64_t word = strtol(token, NULL, 0);
                    insere_uint64_t_no_conteudo(conteudo, indiceConteudo, word);
                }
                uint64_t word = strtoul(token, NULL, 0);
                insere_uint64_t_no_conteudo(conteudo, indiceConteudo, word);
            } else if (strcmp(tipo, "half") == 0) {
                int32_t half = strtol(token, NULL, 0);
                insere_uint32_t_no_conteudo(conteudo, indiceConteudo, half);
            } else if (strcmp(tipo, "quar") == 0) {
                int16_t quarter = strtol(token, NULL, 0);
                insere_uint16_t_no_conteudo(conteudo, indiceConteudo, quarter);
            } else {
                int8_t byte = strtol(token, NULL, 0);
                conteudo[(*indiceConteudo)++] = byte;
            }
        }
        return;
    }

    conteudo[(*indiceConteudo)++] = 1;
    token = strtok (NULL,"\"");
    int64_t quantidade = strlen(token);
    insere_uint64_t_no_conteudo(conteudo, indiceConteudo, quantidade);
    for (int i = 0; i < quantidade; i++) {
        int8_t byte = token[i];
        conteudo[(*indiceConteudo)++] = byte;
    }

    if (strcmp(tipo, "strc") == 0) {
        conteudo[(*indiceConteudo)++] = '\0';
    }
}

void processar_token(
    char *token,
    jump *tabelaJumps,
    int totalJumps,
    uint8_t* conteudo,
    int *indiceConteudo
){
    if (token == NULL) {
        return;
    }

    if (strcmp(token, ".code") == 0) {
        estadoProcessamento = ESTADO_PROCESSAMANETO_CODE;
        *indiceConteudo = TAMANHO_CABECALHO_BINARIO;
        return;
    }

    if (strcmp(token, ".data") == 0) {
        estadoProcessamento = ESTADO_PROCESSAMANETO_DATA;
        *indiceConteudo = tamanhoCode + TAMANHO_CABECALHO_BINARIO;
        return;
    }

    processar_token_code(
        token,
        tabelaJumps,
        totalJumps,
        conteudo,
        indiceConteudo
    );

    processar_token_data(
        token,
        conteudo,
        indiceConteudo
    );
}

uint8_t* gerar_conteudo_binario(
    char *assembly,
    jump *tabelaJumps,
    int totalJumps
) {
    uint8_t *conteudo = malloc(tamanhoArquivo*sizeof(uint8_t));
    char *token;
    int indexConteudo = 4;
    conteudo[0] = 'b';
    conteudo[1] = 'v';
    conteudo[2] = 'm';
    conteudo[3] = 0x01;

    insere_uint32_t_no_conteudo(conteudo, &indexConteudo, tamanhoArquivo);
    insere_uint32_t_no_conteudo(conteudo, &indexConteudo, tamanhoCode);

    token = strtok (assembly," \t\n,");
    do {
        processar_token(
            token,
            tabelaJumps,
            totalJumps,
            conteudo,
            &indexConteudo
        );
        token = strtok (NULL," \t\n,");
    } while(token != NULL);

    return conteudo;
}

uint8_t* processar_arquivo_assembly(
    char* nomeArquivoBass
) {
    FILE *arquivoBass = fopen(nomeArquivoBass, "r");
    if (arquivoBass == NULL) {
        printf("Não foi possível abrir o arquivo: %s!\n", nomeArquivoBass);
        return NULL;
    }

    fseek(arquivoBass, 0, SEEK_END);
    int tamanhoArquivoBytes = ftell(arquivoBass);
    if (tamanhoArquivoBytes > CINCO_MB) {
        printf("O tamanho máximo do arquivo de assembly suportado é 5MB.");
        return NULL;
    }
    fseek(arquivoBass, 0, SEEK_SET);
    char *assembly = malloc((tamanhoArquivoBytes+1) * sizeof(char));
    char ch = fgetc(arquivoBass);
    int indiceAssembly = 0;
    bool comentario = false;
    do {
    	if (ch == '#') {
    		comentario = true;
    	}
    	if (comentario && ch == '\n') {
    		comentario = false;
    		ch = fgetc(arquivoBass);
    		continue;
    	}
    	if (comentario) {
    		ch = fgetc(arquivoBass);
    		continue;
    	}
        assembly[indiceAssembly++] = ch;
        ch = fgetc(arquivoBass);
    } while (ch != EOF);
    assembly[indiceAssembly-1] = '\0';

    int totalJumps = 0;
    tamanhoArquivo = TAMANHO_CABECALHO_BINARIO;
    calcular_tamanho_arquivo(assembly, &totalJumps);

    jump *tabelaJumps = montar_tabela_jumps(assembly, totalJumps);

    uint8_t *conteudo =  gerar_conteudo_binario(
        assembly,
        tabelaJumps,
        totalJumps
    );

    fclose(arquivoBass);

    for (int i = 0; i < totalJumps; i++) {
        free(tabelaJumps[i].label);
    }
    free(tabelaJumps);
    free(assembly);

    return conteudo;
}

void escrever_arquivo_binario(
    char* nomeArquivoBass,
    uint8_t* conteudo,
    int tamanhoArquivo
) {
    int comprimentoNomeArquivo = strlen(nomeArquivoBass);
    char* nomeArquivoBbvm = malloc(comprimentoNomeArquivo+1);
    strcpy(nomeArquivoBbvm, nomeArquivoBass);
    strcpy(&nomeArquivoBbvm[comprimentoNomeArquivo-4], "bbvm");

    FILE *arquivoBbvm = fopen(nomeArquivoBbvm, "w+");

    if (arquivoBbvm == NULL) {
        printf("Erro ao salvar arquivo binário");
        return;
    }

    fwrite(conteudo, tamanhoArquivo, 1, arquivoBbvm);

    fclose(arquivoBbvm);
    free(nomeArquivoBbvm);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("É necessário especificar o arquivo de assembly!\n");
        return 0;
    }

    tamanhoArquivo = 0;
    uint8_t* conteudo = processar_arquivo_assembly(argv[1]);
    if (tamanhoArquivo == 0 && conteudo == NULL ) {
        return 1;
    }
    escrever_arquivo_binario(argv[1], conteudo, tamanhoArquivo);

    free(conteudo);
}
