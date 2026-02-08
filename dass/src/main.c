#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cstrings.h"
#include "instrucoes.h"

#define CINCO_MB 5000000
#define POSICAO_TAMANHO_ARQUIVO 4
#define POSICAO_TAMANHO_PROGRAMA 8
#define TAMANHO_CABECALHO_BINARIO 12
#define TIPO_DADO_64_BITS 8
#define TIPO_DADO_32_BITS 4
#define TIPO_DADO_16_BITS 2
#define TIPO_DADO_8_BITS 1

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

cstrings_t* ler_programa_do_arquivo_binario(char* nomeArquivoBbvm) {
    FILE *arquivoBbvm = fopen(nomeArquivoBbvm, "r");
    fseek(arquivoBbvm, 0, SEEK_END);
    int tamanhoArquivoBytes = ftell(arquivoBbvm);
    if (tamanhoArquivoBytes > CINCO_MB) {
        printf("O tamanho máximo do arquivo de assembly suportado é 5MB.");
        return NULL;
    }
    fseek(arquivoBbvm, 0, SEEK_SET);

    uint8_t buffer[tamanhoArquivoBytes + 1];

    if (arquivoBbvm == NULL) {
        printf("Não foi possível abrir o arquivo: %s!\n", nomeArquivoBbvm);
        return NULL;
    }

    fread(buffer, sizeof(buffer), 1, arquivoBbvm);
    fclose(arquivoBbvm);
    if (!(strncmp((char*)buffer, "bvm", 3) == 0 && buffer[3] == 1)) {
        printf("Programa inválido\n");
        return NULL;
    }

    cstrings_t* assembly = appendToCstrings_t(NULL, ".code\n");

    uint32_t tamanhoPrograma = ler_uint32_t(buffer, POSICAO_TAMANHO_PROGRAMA);
    uint32_t tam_programa = tamanhoPrograma;
    uint32_t i = 0;
    while (i < tamanhoPrograma) {
        uint8_t instrucao = buffer[i + TAMANHO_CABECALHO_BINARIO];
        switch (instrucao) {
            case INST_NOP:
                appendToCstrings_t(assembly, "\tnop\n");
                break;
            case INST_HALT:
                appendToCstrings_t(assembly, "\thalt\n");
                break;
            case INST_PUSH:
                appendToCstrings_t(assembly, "\tpush ");
                uint64_t word = ler_uint64_t(buffer, i + 1 + TAMANHO_CABECALHO_BINARIO);
                i += 8;
                char str_word[64];
                sprintf(str_word, "%lu\n", word);
                appendToCstrings_t(assembly, str_word);
                break;
            case INST_POP:
                appendToCstrings_t(assembly, "\tpop\n");
                break;
            case INST_ADDU:
                appendToCstrings_t(assembly, "\taddu\n");
                break;
            case INST_SUBU:
                appendToCstrings_t(assembly, "\tsubu\n");
                break;
            case INST_MULU:
                appendToCstrings_t(assembly, "\tmulu\n");
                break;
            case INST_DIVU:
                appendToCstrings_t(assembly, "\tdivu\n");
                break;
            case INST_REMU:
                appendToCstrings_t(assembly, "\tremu\n");
                break;
            case INST_ADD:
                appendToCstrings_t(assembly, "\tadd\n");
                break;
            case INST_SUB:
                appendToCstrings_t(assembly, "\tsub\n");
                break;
            case INST_MUL:
                appendToCstrings_t(assembly, "\tmul\n");
                break;
            case INST_DIV:
                appendToCstrings_t(assembly, "\tdiv\n");
                break;
            case INST_REM:
                appendToCstrings_t(assembly, "\trem\n");
                break;
            case INST_ADDF:
                appendToCstrings_t(assembly, "\taddf\n");
                break;
            case INST_SUBF:
                appendToCstrings_t(assembly, "\tsubf\n");
                break;
            case INST_MULF:
                appendToCstrings_t(assembly, "\tmulf\n");
                break;
            case INST_DIVF:
                appendToCstrings_t(assembly, "\tdivf\n");
                break;
            case INST_SL:
                appendToCstrings_t(assembly, "\tsl\n");
                break;
            case INST_SR:
                appendToCstrings_t(assembly, "\tsr\n");
                break;
            case INST_AND:
                appendToCstrings_t(assembly, "\tand\n");
                break;
            case INST_OR:
                appendToCstrings_t(assembly, "\tor\n");
                break;
            case INST_XOR:
                appendToCstrings_t(assembly, "\txor\n");
                break;
            case INST_NOR:
                appendToCstrings_t(assembly, "\tnor\n");
                break;
            case INST_BEQ:
                appendToCstrings_t(assembly, "\tbeq\n");
                break;
            case INST_BNE:
                appendToCstrings_t(assembly, "\tbne\n");
                break;
            case INST_BLTZ:
                appendToCstrings_t(assembly, "\tbltz\n");
                break;
            case INST_BLEZ:
                appendToCstrings_t(assembly, "\tblez\n");
                break;
            case INST_BGTZ:
                appendToCstrings_t(assembly, "\tbgtz\n");
                break;
            case INST_BGEZ:
                appendToCstrings_t(assembly, "\tbgez\n");
                break;
            case INST_CALL:
                appendToCstrings_t(assembly, "\tcall\n");
                break;
            case INST_JUMP:
                appendToCstrings_t(assembly, "\tjump\n");
                break;
            case INST_SW:
                appendToCstrings_t(assembly, "\tsw\n");
                break;
            case INST_SH:
                appendToCstrings_t(assembly, "\tsh\n");
                break;
            case INST_SQ:
                appendToCstrings_t(assembly, "\tsq\n");
                break;
            case INST_SB:
                appendToCstrings_t(assembly, "\tsb\n");
                break;
            case INST_LW:
                appendToCstrings_t(assembly, "\tlw\n");
                break;
            case INST_LH:
                appendToCstrings_t(assembly, "\tlh\n");
                break;
            case INST_LQ:
                appendToCstrings_t(assembly, "\tlq\n");
                break;
            case INST_LB:
                appendToCstrings_t(assembly, "\tlb\n");
                break;
            case INST_EXT:
                appendToCstrings_t(assembly, "\text\n");
                break;
        }
        i++;
    }

    uint32_t tamanhoArquivo = ler_uint32_t(buffer, POSICAO_TAMANHO_ARQUIVO);
    uint32_t inicioSecaoData = TAMANHO_CABECALHO_BINARIO + tamanhoPrograma;

    if (tamanhoArquivo == inicioSecaoData) {
        return assembly;
    }

    i = inicioSecaoData;
    do {
        uint64_t endereco = ler_uint64_t(buffer, i);
        i += 8;
        uint8_t tipo = buffer[i++];
        uint64_t tamanho = ler_uint64_t(buffer, i);
        i += 8;

        char str_buffer[128];
        switch (tipo) {
            case TIPO_DADO_64_BITS:
                sprintf(str_buffer, "%lu word %lu", endereco, tamanho);
                break;
            case TIPO_DADO_32_BITS:
                sprintf(str_buffer, "%lu half %lu", endereco, tamanho);
                break;
            case TIPO_DADO_16_BITS:
                sprintf(str_buffer, "%lu quar %lu", endereco, tamanho);
                break;
            case TIPO_DADO_8_BITS:
                sprintf(str_buffer, "%lu byte %lu", endereco, tamanho);
                break;
        }
        appendToCstrings_t(assembly, str_buffer);

        for (uint32_t j = 0; j < tamanho; j++) {
            if (tipo == TIPO_DADO_64_BITS) {
                uint64_t valor = ler_uint64_t(buffer, i);
                sprintf(str_buffer, " %lu", valor);
                endereco += 8;
                i += 8;
            } else if(tipo == TIPO_DADO_32_BITS) {
                uint32_t valor = ler_uint32_t(buffer, i);
                sprintf(str_buffer, " %u", valor);
                endereco += 4;
                i += 4;
            } else if(tipo == TIPO_DADO_16_BITS) {
                uint16_t valor = ler_uint16_t(buffer, i);
                sprintf(str_buffer, " %u", valor);
                endereco += 2;
                i += 2;
            } else {
                uint8_t valor = buffer[i];
                sprintf(str_buffer, " %u", valor);
                endereco += 1;
                i += 1;
            }
            appendToCstrings_t(assembly, str_buffer);
        }
        appendToCstrings_t(assembly, "\n");
    } while (i < tamanhoArquivo);

    return assembly;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("É necessário especificar o arquivo de assembly!\n");
        return 0;
    }

    cstrings_t *string = ler_programa_do_arquivo_binario(argv[1]);
    printf("Dissabled:\n\n %s", getCstrings_tContent(string));
}
