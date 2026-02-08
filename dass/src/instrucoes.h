#ifndef INSTRUCOES_H
#define INSTRUCOES_H

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

#endif /* INSTRUCOES_H */
