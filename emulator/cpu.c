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

bool load_program(CPU *cpu, uint32_t *program, size_t length) {
    if (length > MEMORY_SIZE / 4) {
        return false;
    }

    for (size_t i = 0; i < length; i++) {
        size_t address = i * 4;
        cpu->memory[address] = (uint8_t)(program[i] & 0xFF);
        cpu->memory[address + 1] = (uint8_t)((program[i] >> 8) & 0xFF);
        cpu->memory[address + 2] = (uint8_t)((program[i] >> 16) & 0xFF);
        cpu->memory[address + 3] = (uint8_t)((program[i] >> 24) & 0xFF);
    }
    return true;
}

CPUStatus cpu_step(CPU *cpu) {
    if (cpu->pc % 4 != 0 || cpu->pc + 4 > MEMORY_SIZE) {
        return CPU_INVALID_PC;
    }

    uint32_t instruction = 
        (uint32_t)cpu->memory[cpu->pc] | 
        ((uint32_t)cpu->memory[cpu->pc + 1] << 8) | 
        ((uint32_t)cpu->memory[cpu->pc + 2] << 16) | 
        ((uint32_t)cpu->memory[cpu->pc + 3] << 24);

    uint32_t fmt =(instruction >> 30) & 0x3;
    uint32_t op = (instruction >> 25) & 0x1F;
    
    uint32_t a = (instruction >> 22) & 0x7;
    uint32_t b = (instruction >> 19) & 0x7;
    uint32_t c = (instruction >> 16) & 0x7;

    uint32_t imm = instruction & 0xFFFF;

    switch (fmt) {
        case FMT_R:
            if (imm != 0) {
                return CPU_INVALID_INSTRUCTION;
            }
            switch (op) {
                case OP_MOV:
                    if (c != 0) {
                        return CPU_INVALID_INSTRUCTION;
                    }
                    cpu->regs[a] = cpu->regs[b];
                    cpu->pc += 4;
                    break;
                case OP_ADD:
                    cpu->regs[a] = cpu->regs[b] + cpu->regs[c];
                    cpu->pc += 4;
                    break;
                case OP_SUB:
                    cpu->regs[a] = cpu->regs[b] - cpu->regs[c];
                    cpu->pc += 4;
                    break;
                case OP_AND:
                    cpu->regs[a] = cpu->regs[b] & cpu->regs[c];
                    cpu->pc += 4;
                    break;
                case OP_OR:
                    cpu->regs[a] = cpu->regs[b] | cpu->regs[c];
                    cpu->pc += 4;
                    break;
                case OP_XOR:
                    cpu->regs[a] = cpu->regs[b] ^ cpu->regs[c];
                    cpu->pc += 4;
                    break;
                case OP_SHL:
                    cpu->regs[a] = cpu->regs[b] << (cpu->regs[c] & 0xF);
                    cpu->pc += 4;
                    break;
                case OP_SHR:
                    cpu->regs[a] = cpu->regs[b] >> (cpu->regs[c] & 0xF);
                    cpu->pc += 4;
                    break;
                default:
                    return CPU_INVALID_INSTRUCTION;
            }
            break;
        case FMT_I:
            if (c != 0) {
                return CPU_INVALID_INSTRUCTION;
            }
            switch (op) {
                case OP_LDI:
                    if (b != 0) {
                        return CPU_INVALID_INSTRUCTION;
                    }
                    cpu->regs[a] = (uint16_t)imm;
                    cpu->pc += 4;
                    break;
                case OP_ADDI:
                    cpu->regs[a] = (uint16_t)(cpu->regs[b] + (int16_t)imm);
                    cpu->pc += 4;
                    break;
                case OP_LD8: {
                    uint16_t address = (uint16_t)(cpu->regs[b] + (int16_t)imm);
                    cpu->regs[a] = (uint16_t)(cpu->memory[address] & 0xFF);
                    cpu->pc += 4;
                    break;
                }
                case OP_LD16: {
                    uint16_t address = (uint16_t)(cpu->regs[b] + (int16_t)imm);
                    if (address % 2 == 0) {
                        cpu->regs[a] = 
                            (uint16_t)cpu->memory[address] |
                            ((uint16_t)cpu->memory[address + 1] << 8);
                        cpu->pc += 4;
                    } else {
                        return CPU_MISALIGNED_ACCESS;
                    }
                    break;
                }
                case OP_ST8: {
                    uint16_t address = (uint16_t)(cpu->regs[b] + (int16_t)imm);
                    cpu->memory[address] = (uint8_t)(cpu->regs[a] & 0xFF);
                    cpu->pc += 4;
                    break;
                }
                case OP_ST16: {
                    uint16_t address = (uint16_t)(cpu->regs[b] + (int16_t)imm);
                    if (address % 2 == 0) {
                        cpu->memory[address] = (uint8_t)(cpu->regs[a] & 0xFF);
                        cpu->memory[address + 1] = (uint8_t)((cpu->regs[a] >> 8) & 0xFF);
                        cpu->pc += 4;
                    } else {
                        return CPU_MISALIGNED_ACCESS;
                    }
                    break;
                }
                default:
                    return CPU_INVALID_INSTRUCTION;
            }
            break;
        case FMT_B:
            if (c != 0 || (int16_t)imm % 4 != 0) {
                return CPU_INVALID_INSTRUCTION;
            }
            switch (op) {
                case OP_BEQ:
                    if (cpu->regs[a] == cpu->regs[b]) {
                        cpu->pc = (uint16_t)(cpu->pc + 4 + (int16_t)imm);
                    } else {
                        cpu->pc += 4;
                    }
                    break;
                case OP_BNE:
                    if (cpu->regs[a] != cpu->regs[b]) {
                        cpu->pc = (uint16_t)(cpu->pc + 4 + (int16_t)imm);
                    } else {
                        cpu->pc += 4;
                    }
                    break;
                case OP_BLT:
                    if ((int16_t)cpu->regs[a] < (int16_t)cpu->regs[b]) {
                        cpu->pc = (uint16_t)(cpu->pc + 4 + (int16_t)imm);
                    } else {
                        cpu->pc += 4;
                    }
                    break;
                case OP_BGE:
                    if ((int16_t)cpu->regs[a] >= (int16_t)cpu->regs[b]) {
                        cpu->pc = (uint16_t)(cpu->pc + 4 + (int16_t)imm);
                    } else {
                        cpu->pc += 4;
                    }
                    break;
                default:
                    return CPU_INVALID_INSTRUCTION;
            }
            break;
        case FMT_J:
            switch (op) {
                case OP_JMP:
                    if (a != 0 || b != 0 || c != 0 || (int16_t)imm % 4 != 0) {
                        return CPU_INVALID_INSTRUCTION;
                    }
                    cpu->pc = (uint16_t)(cpu->pc + 4 + (int16_t)imm);
                    break;
                case OP_JAL:
                    if (b != 0 || c != 0 || (int16_t)imm % 4 != 0) {
                        return CPU_INVALID_INSTRUCTION;
                    }
                    cpu->regs[a] = cpu->pc + 4;
                    cpu->pc = (uint16_t)(cpu->pc + 4 + (int16_t)imm);
                    break;
                case OP_JR:
                    if (b != 0 || c != 0 || imm != 0) {
                        return CPU_INVALID_INSTRUCTION;
                    }
                    cpu->pc = cpu->regs[a];
                    break;
                case OP_JALR:
                    if (c != 0 || imm != 0) {
                        return CPU_INVALID_INSTRUCTION;
                    }
                    uint16_t target = cpu->regs[b];
                    cpu->regs[a] = cpu->pc + 4;
                    cpu->pc = target;
                    break;
                case OP_HALT:
                    if (a != 0 || b != 0 || c != 0 || imm != 0) {
                        return CPU_INVALID_INSTRUCTION;
                    }
                    cpu->halted = true;
                    return CPU_HALTED;
                case OP_TRAP:
                    if (a != 0 || b != 0 || c != 0) {
                        return CPU_INVALID_INSTRUCTION;
                    }
                    switch (imm) {
                        case 1:
                            printf("%d", (int16_t)(cpu->regs[0]));
                            break;
                        default:
                            return CPU_UNSUPPORTED_TRAP;
                    } 
                    cpu->pc += 4;
                    break;
                default:
                    return CPU_INVALID_INSTRUCTION;
            }
            break;
    }
    return CPU_OK;
}

CPUStatus cpu_run(CPU *cpu, size_t max_steps) {
    for (size_t i = 0; i < max_steps; i++) {
        CPUStatus status = cpu_step(cpu);

        if (status != CPU_OK) {
            return status;
        }
    }

    return CPU_STEP_LIMIT;
}