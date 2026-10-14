#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define NUM_REGS 8
#define MEMORY_SIZE 65536
#define MAX_STEPS 10000

typedef struct {
    uint16_t regs[NUM_REGS];
    uint16_t pc;
    uint8_t memory[MEMORY_SIZE];
    bool halted;
} CPU;

typedef enum {
    R_FMT,
    I_FMT,
    B_FMT,
    J_FMT
} Fmt;

typedef enum {
    MOV_OP,
    ADD_OP,
    SUB_OP,
    AND_OP,
    OR_OP,
    XOR_OP,
    SHL_OP,
    SHR_OP
} OpR;

typedef enum {
    LDI_OP,
    ADDI_OP,
    LD8_OP,
    LD16_OP,
    ST8_OP,
    ST16_OP
} OpI;

typedef enum {
    BEQ_OP,
    BNE_OP,
    BLT_OP,
    BGE_OP
} OpB;

typedef enum {
    JMP_OP,
    JAL_OP,
    JR_OP,
    JALR_OP,
    HALT_OP,
    TRAP_OP
} OpJ;

#endif