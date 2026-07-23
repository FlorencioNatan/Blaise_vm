#include "instrucoes.h"
#include <string.h>

registroInstrucao instrucoes[QUANTIDADE_INSTRUCOES] = {
    {INST_CMD_NOP , INST_NOP ,  false, false,  1},
    {INST_CMD_HALT, INST_HALT,  false, false,  1},
    {INST_CMD_PUSH, INST_PUSH,  true,  false,  9},
    {INST_CMD_POP , INST_POP ,  false, false,  1},
    {INST_CMD_ADDU, INST_ADDU,  false, false,  1},
    {INST_CMD_SUBU, INST_SUBU,  false, false,  1},
    {INST_CMD_MULU, INST_MULU,  false, false,  1},
    {INST_CMD_DIVU, INST_DIVU,  false, false,  1},
    {INST_CMD_REMU, INST_REMU,  false, false,  1},
    {INST_CMD_ADD , INST_ADD ,  false, false,  1},
    {INST_CMD_SUB , INST_SUB ,  false, false,  1},
    {INST_CMD_MUL , INST_MUL ,  false, false,  1},
    {INST_CMD_DIV , INST_DIV ,  false, false,  1},
    {INST_CMD_REM , INST_REM ,  false, false,  1},
    {INST_CMD_ADDF, INST_ADDF,  false, false,  1},
    {INST_CMD_SUBF, INST_SUBF,  false, false,  1},
    {INST_CMD_MULF, INST_MULF,  false, false,  1},
    {INST_CMD_DIVF, INST_DIVF,  false, false,  1},

    {INST_CMD_SL  , INST_SL  ,  false, false,  1},
    {INST_CMD_SR  , INST_SR  ,  false, false,  1},
    {INST_CMD_AND , INST_AND ,  false, false,  1},
    {INST_CMD_OR  , INST_OR  ,  false, false,  1},
    {INST_CMD_XOR , INST_XOR ,  false, false,  1},
    {INST_CMD_NOR , INST_NOR ,  false, false,  1},

    {INST_CMD_BEQ , INST_BEQ ,  false, false,  1},
    {INST_CMD_BNE , INST_BNE ,  false, false,  1},
    {INST_CMD_BLTZ, INST_BLTZ,  false, false,  1},
    {INST_CMD_BLEZ, INST_BLEZ,  false, false,  1},
    {INST_CMD_BGTZ, INST_BGTZ,  false, false,  1},
    {INST_CMD_BGEZ, INST_BGEZ,  false, false,  1},
    {INST_CMD_CALL, INST_CALL,  false, false,  1},
    {INST_CMD_JUMP, INST_JUMP,  false, false,  1},
    {INST_CMD_SW,   INST_SW,    false, false,  1},
    {INST_CMD_SH,   INST_SH,    false, false,  1},
    {INST_CMD_SQ,   INST_SQ,    false, false,  1},
    {INST_CMD_SB,   INST_SB,    false, false,  1},
    {INST_CMD_LW,   INST_LW,    false, false,  1},
    {INST_CMD_LH,   INST_LH,    false, false,  1},
    {INST_CMD_LQ,   INST_LQ,    false, false,  1},
    {INST_CMD_LB,   INST_LB,    false, false,  1},

    {INST_CMD_BEQI , INST_BEQ , true, false,  10},
    {INST_CMD_BNEI , INST_BNE , true, false,  10},
    {INST_CMD_BLTZI, INST_BLTZ, true, false,  10},
    {INST_CMD_BLEZI, INST_BLEZ, true, false,  10},
    {INST_CMD_BGTZI, INST_BGTZ, true, false,  10},
    {INST_CMD_BGEZI, INST_BGEZ, true, false,  10},
    {INST_CMD_CALLI, INST_CALL, true, false,  10},
    {INST_CMD_JUMPI, INST_JUMP, true, false,  10},
    {INST_CMD_SWI,   INST_SW,   true, false,  10},
    {INST_CMD_SHI,   INST_SH,   true, false,  10},
    {INST_CMD_SQI,   INST_SQ,   true, false,  10},
    {INST_CMD_SBI,   INST_SB,   true, false,  10},
    {INST_CMD_LWI,   INST_LW,   true, false,  10},
    {INST_CMD_LHI,   INST_LH,   true, false,  10},
    {INST_CMD_LQI,   INST_LQ,   true, false,  10},
    {INST_CMD_LBI,   INST_LB,   true, false,  10},
    {INST_CMD_EXT , INST_EXT ,  false, false,  1},

	{INST_CMD_DBGLNI,  INST_DBGLNI ,  false, true,  0},
	{INST_CMD_DBGLNF,  INST_DBGLNF ,  false, true,  0},
	{INST_CMD_DBGVARI, INST_DBGVARI ,  false, true,  0},
	{INST_CMD_DBGVAR,  INST_DBGVAR ,  false, true,  0},
};

registroInstrucao lookup_instrucao(char* instrucao) {
    for (int i = 0; i < QUANTIDADE_INSTRUCOES; i++) {
        if (strcmp(instrucoes[i].cmd, instrucao) == 0) {
            return instrucoes[i];
        }
    }

    return instrucoes[0];
}
