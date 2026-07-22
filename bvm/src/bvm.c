#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "bvm.h"
#include "instrucoes.h"
#include "ext.h"

void escreve_uint64_t_na_memoria(bvm *vm, uint64_t endereco, uint64_t valor) {
    uint8_t *bytesValor = (uint8_t*)&valor;

    for(uint64_t i = 0; i < sizeof(uint64_t); i++) {
        vm->memoria[endereco+i] = bytesValor[i];
    }
}

void escreve_uint32_t_na_memoria(bvm *vm, uint64_t endereco, uint32_t valor) {
    uint8_t *bytesValor = (uint8_t*)&valor;

    for(uint64_t i = 0; i < sizeof(uint32_t); i++) {
        vm->memoria[endereco+i] = bytesValor[i];
    }
}

void escreve_uint16_t_na_memoria(bvm *vm, uint64_t endereco, uint16_t valor) {
    uint8_t *bytesValor = (uint8_t*)&valor;

    for(uint64_t i = 0; i < sizeof(uint16_t); i++) {
        vm->memoria[endereco+i] = bytesValor[i];
    }
}

void escreve_double_na_memoria(bvm *vm, uint64_t endereco, double valor) {
    uint8_t *bytesValor = (uint8_t*)&valor;

    for(uint64_t i = 0; i < sizeof(uint64_t); i++) {
        vm->memoria[endereco+i] = bytesValor[i];
    }
}

uint64_t le_uint64_t_na_memoria(bvm *vm, uint64_t endereco) {
    uint64_t valor = 0;
    for (uint64_t i = 64; i >= 8;i -= 8) {
        valor |= ((uint64_t) vm->memoria[endereco++]) << (64 - i);
    }
    return valor;
}

uint32_t le_uint32_t_na_memoria(bvm *vm, uint64_t endereco) {
    uint32_t valor = 0;
    for (uint64_t i = 32; i >= 8;i -= 8) {
        valor |= ((uint64_t) vm->memoria[endereco++]) << (32 - i);
    }
    return valor;
}

uint16_t le_uint16_t_na_memoria(bvm *vm, uint64_t endereco) {
    uint16_t valor = 0;
    for (uint64_t i = 16; i >= 8;i -= 8) {
        valor |= ((uint64_t) vm->memoria[endereco++]) << (16 - i);
    }
    return valor;
}

int le_int_da_memoria(bvm *vm, uint64_t endereco) {
    int valor = 0;
    for (int i = 32; i > 0;i -= 8) {
        valor |= ((int) vm->memoria[endereco++]) << (32 - i);
    }
    return valor;
}

double le_double_da_memoria(bvm *vm, uint64_t endereco) {
    uint64_t valor = 0;
    double *valorDouble = (double*) &valor;
    for (uint64_t i = 64; i >= 8;i -= 8) {
        valor |= ((uint64_t) vm->memoria[endereco++]) << (64 - i);
    }
    return *valorDouble;
}

char* le_string_da_memoria(bvm *vm, uint64_t endereco, int tamanho) {
    char* valor;
    valor = malloc(tamanho + 1);
    for (int i = 0; i < tamanho; i++) {
        valor[i] = vm->memoria[endereco++];
    }
    valor[tamanho] = '\0';
    return valor;
}

int operacao_binaria_u(bvm *vm, char operador) {
    if (vm->tam_pilha < 2) {
        return EXEC_ERRO_STACK_UNDERFLOW;
    }

    uint64_t operando2 = vm->pilha[--vm->tam_pilha];
    uint64_t operando1 = vm->pilha[--vm->tam_pilha];

    switch (operador) {
        case '+':
            vm->pilha[vm->tam_pilha++] = (operando1 + operando2);
            break;
        case '-':
            vm->pilha[vm->tam_pilha++] = (operando1 - operando2);
            break;
        case '*':
            vm->pilha[vm->tam_pilha++] = (operando1 * operando2);
            break;
        case '/':
            if (operando2 == 0) {
                return EXEC_ERRO_DIVISAO_POR_ZERO;
            }

            vm->pilha[vm->tam_pilha++] = (operando1 / operando2);
            break;
        case '%':
            vm->pilha[vm->tam_pilha++] = (operando1 % operando2);
            break;
    }

    return EXEC_SUCESSO;
}

int operacao_binaria_i(bvm *vm, char operador) {
    if (vm->tam_pilha < 2) {
        return EXEC_ERRO_STACK_UNDERFLOW;
    }

    int64_t operando2 = vm->pilha[--vm->tam_pilha];
    int64_t operando1 = vm->pilha[--vm->tam_pilha];

    switch (operador) {
        case '+':
            vm->pilha[vm->tam_pilha++] = (operando1 + operando2);
            break;
        case '-':
            vm->pilha[vm->tam_pilha++] = (operando1 - operando2);
            break;
        case '*':
            vm->pilha[vm->tam_pilha++] = (operando1 * operando2);
            break;
        case '/':
            if (operando2 == 0) {
                return EXEC_ERRO_DIVISAO_POR_ZERO;
            }

            vm->pilha[vm->tam_pilha++] = (operando1 / operando2);
            break;
        case '%':
            vm->pilha[vm->tam_pilha++] = (operando1 % operando2);
            break;
    }

    return EXEC_SUCESSO;
}

int operacao_binaria_f(bvm *vm, char operador) {
    if (vm->tam_pilha < 2) {
        return EXEC_ERRO_STACK_UNDERFLOW;
    }

    double operando2 = vm->pilha[--vm->tam_pilha];
    double operando1 = vm->pilha[--vm->tam_pilha];

    switch (operador) {
        case '+':
            vm->pilha[vm->tam_pilha++] = (operando1 + operando2);
            break;
        case '-':
            vm->pilha[vm->tam_pilha++] = (operando1 - operando2);
            break;
        case '*':
            vm->pilha[vm->tam_pilha++] = (operando1 * operando2);
            break;
        case '/':
            if (operando2 == 0) {
                return EXEC_ERRO_DIVISAO_POR_ZERO;
            }

            vm->pilha[vm->tam_pilha++] = (operando1 / operando2);
            break;
    }

    return EXEC_SUCESSO;
}

int operacao_binaria_bw(bvm *vm, char operador) {
    if (vm->tam_pilha < 2) {
        return EXEC_ERRO_STACK_UNDERFLOW;
    }

    uint64_t operando2 = vm->pilha[--vm->tam_pilha];
    uint64_t operando1 = vm->pilha[--vm->tam_pilha];

    switch (operador) {
        case '>':
            vm->pilha[vm->tam_pilha++] = (operando1 >> operando2);
            break;
        case '<':
            vm->pilha[vm->tam_pilha++] = (operando1 << operando2);
            break;
        case '&':
            vm->pilha[vm->tam_pilha++] = (operando1 & operando2);
            break;
        case '|':
            vm->pilha[vm->tam_pilha++] = (operando1 | operando2);
            break;
        case '^':
            vm->pilha[vm->tam_pilha++] = (operando1 ^ operando2);
            break;
        case '~':
            vm->pilha[vm->tam_pilha++] = ~(operando1 | operando2);
            break;
    }

    return EXEC_SUCESSO;
}

int jump(bvm *vm) {
    if (vm->tam_pilha < 1) {
        return EXEC_ERRO_STACK_UNDERFLOW;
    }

    uint64_t operando1 = vm->pilha[--vm->tam_pilha];
    vm->pc = operando1;

    return EXEC_SUCESSO;
}

int call(bvm *vm) {
    if (vm->tam_pilha < 1) {
        return EXEC_ERRO_STACK_UNDERFLOW;
    }

    uint64_t operando1 = vm->pilha[vm->tam_pilha-1];
    vm->pilha[vm->tam_pilha-1] = vm->pc;
    vm->pc = operando1;

    return EXEC_SUCESSO;
}

int branch_condicional(bvm *vm, char condicao) {
    if (vm->tam_pilha < 2) {
        return EXEC_ERRO_STACK_UNDERFLOW;
    }

    int64_t operando2 = vm->pilha[--vm->tam_pilha];
    int64_t operando1 = vm->pilha[--vm->tam_pilha];

    switch (condicao) {
        case 'e':
            if (operando1 == 0) {
                vm->pc = operando2;
            }
            break;
        case 'n':
            if (operando1 != 0) {
                vm->pc = operando2;
            }
            break;
        case 'L':
            if (operando1 < 0) {
                vm->pc = operando2;
            }
            break;
        case 'l':
            if (operando1 <= 0) {
                vm->pc = operando2;
            }
            break;
        case 'G':
            if (operando1 > 0) {
                vm->pc = operando2;
            }
            break;
        case 'g':
            if (operando1 >= 0) {
                vm->pc = operando2;
            }
            break;
    }

    return EXEC_SUCESSO;
}

int operacao_store(bvm *vm, char tamanho) {
    if (vm->tam_pilha < 2) {
        return EXEC_ERRO_STACK_UNDERFLOW;
    }

    int64_t operando2 = vm->pilha[--vm->tam_pilha];
    int64_t operando1 = vm->pilha[--vm->tam_pilha];

    if (operando2 < 0 || operando2 > MAX_TAM_MEMORIA) {
        return EXEC_ERRO_ACESSO_INVALIDO_A_MEMORIA;
    }

    switch (tamanho) {
        case 'w':
            escreve_uint64_t_na_memoria(vm, operando2, operando1);
            break;
        case 'h':
            escreve_uint32_t_na_memoria(vm, operando2, (uint32_t)operando1);
            break;
        case 'q':
            escreve_uint16_t_na_memoria(vm, operando2, (uint16_t)operando1);
            break;
        case 'b':
            vm->memoria[operando2] = (uint8_t)operando1;
            break;
    }
    return EXEC_SUCESSO;
}

int operacao_load(bvm *vm, char tamanho) {
    if (vm->tam_pilha < 1) {
        return EXEC_ERRO_STACK_UNDERFLOW;
    }

    int64_t operando1 = vm->pilha[--vm->tam_pilha];

    if (operando1 < 0 || operando1 > MAX_TAM_MEMORIA) {
        return EXEC_ERRO_ACESSO_INVALIDO_A_MEMORIA;
    }

    switch (tamanho) {
        case 'w':
            vm->pilha[vm->tam_pilha++] = le_uint64_t_na_memoria(vm, operando1);
            break;
        case 'h':
            vm->pilha[vm->tam_pilha++] = le_uint32_t_na_memoria(vm, operando1);
            break;
        case 'q':
            vm->pilha[vm->tam_pilha++] = le_uint16_t_na_memoria(vm, operando1);
            break;
        case 'b':
            vm->pilha[vm->tam_pilha++] = ((uint64_t) vm->memoria[operando1++]);
            break;
    }

    return EXEC_SUCESSO;
}

int extensao(bvm *vm) {
    if (vm->tam_pilha < 3) {
        return EXEC_ERRO_STACK_UNDERFLOW;
    }

    uint64_t fim = vm->pilha[--vm->tam_pilha];
    uint64_t inicio = vm->pilha[--vm->tam_pilha];
    uint64_t extensao = vm->pilha[--vm->tam_pilha];

    return processar_extensao(extensao, inicio, fim, vm);
}

int processar_instrucoes(bvm *vm) {
    uint8_t inst = vm->programa[vm->pc++];
    uint64_t imediato = 0;

    printf("Inst: 0x%02x\n", inst);
    switch (inst) {
        case INST_NOP:
            break;
        case INST_HALT:
            vm->halt = 1;
            break;
        case INST_PUSH:
            if (vm->tam_pilha == MAX_TAM_PILHA) {
                return EXEC_ERRO_STACK_OVERFLOW;
            }

            for (uint64_t i = 64; i >= 8;i -= 8) {
                imediato |= ((uint64_t) vm->programa[vm->pc++]) << (64 - i);
            }
            vm->pilha[vm->tam_pilha++] = imediato;
            break;
        case INST_POP:
            if (vm->tam_pilha == 0) {
                return EXEC_ERRO_STACK_UNDERFLOW;
            }

            imediato =  vm->pilha[vm->tam_pilha - 1];
            vm->tam_pilha--;
            break;

        case INST_ADDU:
            return operacao_binaria_u(vm, '+');
            break;
        case INST_SUBU:
            return operacao_binaria_u(vm, '-');
            break;
        case INST_MULU:
            return operacao_binaria_u(vm, '*');
            break;
        case INST_DIVU:
            return operacao_binaria_u(vm, '/');
            break;
        case INST_REMU:
            return operacao_binaria_u(vm, '%');
            break;

        case INST_ADD:
            return operacao_binaria_i(vm, '+');
            break;
        case INST_SUB:
            return operacao_binaria_i(vm, '-');
            break;
        case INST_MUL:
            return operacao_binaria_i(vm, '*');
            break;
        case INST_DIV:
            return operacao_binaria_i(vm, '/');
            break;
        case INST_REM:
            return operacao_binaria_i(vm, '%');
            break;

        case INST_ADDF:
            return operacao_binaria_f(vm, '+');
            break;
        case INST_SUBF:
            return operacao_binaria_f(vm, '-');
            break;
        case INST_MULF:
            return operacao_binaria_f(vm, '*');
            break;
        case INST_DIVF:
            return operacao_binaria_f(vm, '/');
            break;

        case INST_SL:
            return operacao_binaria_bw(vm, '<');
            break;
        case INST_SR:
            return operacao_binaria_bw(vm, '>');
            break;
        case INST_AND:
            return operacao_binaria_bw(vm, '&');
            break;
        case INST_OR:
            return operacao_binaria_bw(vm, '|');
            break;
        case INST_XOR:
            return operacao_binaria_bw(vm, '^');
            break;
        case INST_NOR:
            return operacao_binaria_bw(vm, '~');
            break;

        case INST_BEQ:
            return branch_condicional(vm, 'e');
            break;
        case INST_BNE:
            return branch_condicional(vm, 'n');
            break;
        case INST_BLTZ:
            return branch_condicional(vm, 'L');
            break;
        case INST_BLEZ:
            return branch_condicional(vm, 'l');
            break;
        case INST_BGTZ:
            return branch_condicional(vm, 'G');
            break;
        case INST_BGEZ:
            return branch_condicional(vm, 'g');
            break;
        case INST_CALL:
            return call(vm);
            break;
        case INST_JUMP:
            return jump(vm);
            break;

        case INST_SW:
            return operacao_store(vm, 'w');
            break;
        case INST_SH:
            return operacao_store(vm, 'h');
            break;
        case INST_SQ:
            return operacao_store(vm, 'q');
            break;
        case INST_SB:
            return operacao_store(vm, 'b');
            break;

        case INST_LW:
            return operacao_load(vm, 'w');
            break;
        case INST_LH:
            return operacao_load(vm, 'h');
            break;
        case INST_LQ:
            return operacao_load(vm, 'q');
            break;
        case INST_LB:
            return operacao_load(vm, 'b');
            break;
        case INST_EXT:
            return extensao(vm);
            break;

        default:
            return EXEC_ERRO_INSTRUCAO_INVALIDA;
            break;
    }

    printf("Dump Pilha:\n");
    for(uint64_t i = 0; i < vm->tam_pilha; i++) {
        printf("%016lu -> 0x%016lx\n", i, vm->pilha[i]);
    }
    printf("PC: %lu\n", vm->pc);

    return EXEC_SUCESSO;
}
