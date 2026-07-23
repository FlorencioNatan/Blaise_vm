#ifndef INSTRUCOES_H
#define INSTRUCOES_H

#include <stdint.h>
#include <stdbool.h>

#define QUANTIDADE_INSTRUCOES 61

#define INST_NOP  0x00
#define INST_HALT 0x01
#define INST_PUSH 0x02
#define INST_POP  0x03

#define INST_ADDU 0x04
#define INST_SUBU 0x05
#define INST_MULU 0x06
#define INST_DIVU 0x07
#define INST_REMU 0x08

#define INST_ADD  0x09
#define INST_SUB  0x0A
#define INST_MUL  0x0B
#define INST_DIV  0x0C
#define INST_REM  0x0D

#define INST_ADDF 0x0E
#define INST_SUBF 0x0F
#define INST_MULF 0x10
#define INST_DIVF 0x11

#define INST_SL   0x12
#define INST_SR   0x13
#define INST_AND  0x14
#define INST_OR   0x15
#define INST_XOR  0x16
#define INST_NOR  0x17

#define INST_BEQ  0x18
#define INST_BNE  0x19
#define INST_BLTZ 0x1A
#define INST_BLEZ 0x1B
#define INST_BGTZ 0x1C
#define INST_BGEZ 0x1D
#define INST_CALL 0x1E
#define INST_JUMP 0x1F

#define INST_SW   0x20
#define INST_SH   0x21
#define INST_SQ   0x22
#define INST_SB   0x23

#define INST_LW   0x24
#define INST_LH   0x25
#define INST_LQ   0x26
#define INST_LB   0x27

#define INST_EXT  0x28

#define INST_DBGLNI   0xd0
#define INST_DBGLNF   0xd1
#define INST_DBGVARI  0xd2
#define INST_DBGVAR   0xd3



#define INST_CMD_NOP  "nop"
#define INST_CMD_HALT "halt"
#define INST_CMD_PUSH "push"
#define INST_CMD_POP  "pop"

#define INST_CMD_ADDU "addu"
#define INST_CMD_SUBU "subu"
#define INST_CMD_MULU "mulu"
#define INST_CMD_DIVU "divu"
#define INST_CMD_REMU "remu"

#define INST_CMD_ADD  "add"
#define INST_CMD_SUB  "sub"
#define INST_CMD_MUL  "mul"
#define INST_CMD_DIV  "div"
#define INST_CMD_REM  "rem"

#define INST_CMD_ADDF "addf"
#define INST_CMD_SUBF "subf"
#define INST_CMD_MULF "mulf"
#define INST_CMD_DIVF "divf"

#define INST_CMD_SL   "sl"
#define INST_CMD_SR   "sr"
#define INST_CMD_AND  "and"
#define INST_CMD_OR   "or"
#define INST_CMD_XOR  "xor"
#define INST_CMD_NOR  "nor"

#define INST_CMD_BEQ  "beq"
#define INST_CMD_BNE  "bne"
#define INST_CMD_BLTZ "bltz"
#define INST_CMD_BLEZ "blez"
#define INST_CMD_BGTZ "bgtz"
#define INST_CMD_BGEZ "bgez"
#define INST_CMD_CALL "call"
#define INST_CMD_JUMP "jump"

#define INST_CMD_SW   "sw"
#define INST_CMD_SH   "sh"
#define INST_CMD_SQ   "sq"
#define INST_CMD_SB   "sb"

#define INST_CMD_LW   "lw"
#define INST_CMD_LH   "lh"
#define INST_CMD_LQ   "lq"
#define INST_CMD_LB   "lb"

#define INST_CMD_EXT  "ext"

// Pseudo instruções
#define INST_CMD_BEQI  "beqi"
#define INST_CMD_BNEI  "bnei"
#define INST_CMD_BLTZI "bltzi"
#define INST_CMD_BLEZI "blezi"
#define INST_CMD_BGTZI "bgtzi"
#define INST_CMD_BGEZI "bgezi"
#define INST_CMD_CALLI "calli"
#define INST_CMD_JUMPI "jumpi"

#define INST_CMD_SWI   "swi"
#define INST_CMD_SHI   "shi"
#define INST_CMD_SQI   "sqi"
#define INST_CMD_SBI   "sbi"

#define INST_CMD_LWI   "lwi"
#define INST_CMD_LHI   "lhi"
#define INST_CMD_LQI   "lqi"
#define INST_CMD_LBI   "lbi"

#define INST_CMD_DBGLNI   "dbglni"
#define INST_CMD_DBGLNF   "dbglnf"
#define INST_CMD_DBGVARI  "dbgvari"
#define INST_CMD_DBGVAR   "dbgvar"



typedef struct registroInstrucao {
	char *cmd;
	uint8_t codigo;
	bool tem_parametro;
	bool e_debug;
	uint8_t tamanho;
} registroInstrucao;

registroInstrucao lookup_instrucao(char *instrucao);

#endif /* INSTRUCOES_H */
