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
    FMT_R,
    FMT_I,
    FMT_B,
    FMT_J
} Fmt;

typedef enum {
    OP_MOV,
    OP_ADD,
    OP_SUB,
    OP_AND,
    OP_OR,
    OP_XOR,
    OP_SHL,
    OP_SHR
} OpR;

typedef enum {
    OP_LDI,
    OP_ADDI,
    OP_LD8,
    OP_LD16,
    OP_ST8,
    OP_ST16
} OpI;

typedef enum {
    OP_BEQ,
    OP_BNE,
    OP_BLT,
    OP_BGE
} OpB;

typedef enum {
    OP_JMP,
    OP_JAL,
    OP_JR,
    OP_JALR,
    OP_HALT,
    OP_TRAP
} OpJ;

typedef enum {
    CPU_OK,
    CPU_HALTED,
    CPU_INVALID_PC,
    CPU_INVALID_INSTRUCTION,
    CPU_MISALIGNED_ACCESS,
    CPU_UNSUPPORTED_TRAP,
    CPU_STEP_LIMIT
} CPUStatus;

void cpu_init(CPU *cpu);

bool load_program(CPU *cpu, uint32_t *program, size_t length);

CPUStatus cpu_step(CPU *cpu);

CPUStatus cpu_run(CPU *cpu, size_t max_steps);

void display(CPU *cpu);

uint32_t build_instruction(uint32_t fmt, uint32_t op, uint32_t a, uint32_t b, uint32_t c, uint32_t imm);

#endif