#include "cpu.h"

void cpu_init(CPU *cpu) {
    for (size_t i = 0; i < NUM_REGS; i++) {
        cpu->regs[i] = 0;
    }
    cpu->pc = 0;
    for (size_t i = 0; i < MEMORY_SIZE; i++) {
        cpu->memory[i] = 0;
    }
    cpu->halted = false;
}

void load_program(CPU *cpu, uint32_t *program, size_t length) {
    if (length * 4 > MEMORY_SIZE) {
        puts("Out of memory");
        return;
    }

    for (size_t i = 0; i < length; i++) {
        size_t address = i * 4;
        cpu->memory[address] = program[i] & 0xFF;
        cpu->memory[address + 1] = (program[i] >> 8) & 0xFF;
        cpu->memory[address + 2] = (program[i] >> 16) & 0xFF;
        cpu->memory[address + 3] = (program[i] >> 24) & 0xFF;
    }
}

bool step(CPU *cpu) {
    if (cpu->halted) {
        return false;
    }

    if (cpu->pc % 4 != 0 || cpu->pc + 4 > MEMORY_SIZE) {
        puts("Invalid PC");
        return false;
    }

    uint32_t instruction = 
        (uint32_t)cpu->memory[cpu->pc] | 
        ((uint32_t)cpu->memory[cpu->pc + 1] << 8) | 
        ((uint32_t)cpu->memory[cpu->pc + 2] << 16) | 
        ((uint32_t)cpu->memory[cpu->pc + 3] << 24);

    uint8_t fmt = (instruction >> 30) & 0b11;
    uint8_t op = (instruction >> 25) & 0b11111;
    
    uint8_t a = (instruction >> 22) & 0b111;
    uint8_t b = (instruction >> 19) & 0b111;
    uint8_t c = (instruction >> 16) & 0b111;

    uint16_t imm = instruction & 0xFFFF;

    switch (fmt) {
        case R_FMT:
            switch (op) {
                case MOV_OP:
                    cpu->regs[a] = cpu->regs[b];
                    cpu->pc += 4;
                    return true;
                case ADD_OP:
                    cpu->regs[a] = cpu->regs[b] + cpu->regs[c];
                    cpu->pc += 4;
                    return true;
                case SUB_OP:
                    cpu->regs[a] = cpu->regs[b] - cpu->regs[c];
                    cpu->pc += 4;
                    return true;
                default:
                    return false;
            }
            break;
        case I_FMT:
            switch (op) {
                case LDI_OP:
                    cpu->regs[a] = imm;
                    cpu->pc += 4;
                    return true;
                case ADDI_OP:
                    cpu->regs[a] = cpu->regs[b] + (int16_t)imm;
                    cpu->pc += 4;
                    return true;
                case LD8_OP:
                    return false;
                case LD16_OP:
                    return false;
                case ST8_OP:
                    return false;
                case ST16_OP:
                    return false;
                default:
                    return false;
            }
            break;
        case B_FMT:
            switch (op) {
                case BEQ_OP:
                    return false;
                case BNE_OP:
                    return false;
                case BLT_OP:
                    return false;
                case BGE_OP:
                    return false;
                default:
                    return false;
            }
            break;
        case J_FMT:
            switch (op) {
                case JMP_OP:
                    return false;
                case JAL_OP:
                    return false;
                case JR_OP:
                    return false;
                case JALR_OP:
                    return false;
                case HALT_OP:
                    cpu->halted = true;
                    return true;
                case TRAP_OP:
                    return false;
                default:
                    return false;
            }
            break;
        default: 
            return false;
    }
}

void run(CPU *cpu) {
    bool result = true;
    size_t i = 0;
    while (i < MAX_STEPS && result && !(cpu->halted)) {
        result = step(cpu);
        i += 1;
    }
    if (i >= MAX_STEPS) {
        puts("Execution limit exceeded");
    }
    if (!result) {
        printf("Step %zu failed\n", i);
    }
}

void display(CPU *cpu) {
    for (size_t i = 0; i < NUM_REGS; i++) {
        printf("regs[%zu]: %u\n", i, cpu->regs[i]);
    }
    printf("PC: %u\n", cpu->pc);
    if (cpu->halted) {
        puts("halted: true");
    } else {
        puts("halted: false");
    }
}

uint32_t build_instruction(uint32_t fmt, uint32_t op, uint32_t a, uint32_t b, uint32_t c, uint32_t imm) {
    uint32_t instruction = 
        (fmt << 30) | 
        (op << 25) |
        (a << 22) |
        (b << 19) |
        (c << 16) |
        imm & 0xFFFF;
    return instruction;
}