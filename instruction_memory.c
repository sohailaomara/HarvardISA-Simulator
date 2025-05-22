#include "instruction_memory.h"
#include <stdio.h>

// Global instruction memory instance
InstructionMemory imem;

// Initialize instruction memory with default values
void init_instruction_memory(InstructionMemory *imem) {
    for (int i = 0; i < INSTR_MEM_SIZE; i++) {
        imem->memory[i] = 0x0000; // Initialize with NOPs or default value
    }
}

// Read instruction from memory (word-addressable)
instruction_t read_instruction(InstructionMemory *imem, uint16_t address) {
    if (address >= INSTR_MEM_SIZE) {
        printf("Error: Instruction memory address out of bounds (0x%04X)\n", address);
        return 0x0000; // Return NOP on error
    }
    return imem->memory[address];
}

// Write instruction to memory (word-addressable)
void write_instruction(InstructionMemory *imem, uint16_t address, instruction_t instruction) {
    if (address >= INSTR_MEM_SIZE) {
        printf("Error: Instruction memory address out of bounds (0x%04X)\n", address);
        return;
    }
    imem->memory[address] = instruction;
}

// Print a range of instruction memory
void print_instruction_memory(InstructionMemory *imem, uint16_t start_addr, uint16_t end_addr) {
    if (start_addr > end_addr || end_addr >= INSTR_MEM_SIZE) {
        printf("Error: Invalid address range\n");
        return;
    }
    
    printf("Instruction Memory Contents [0x%04X - 0x%04X]:\n", start_addr, end_addr);
    for (uint16_t addr = start_addr; addr <= end_addr; addr++) {
        printf("IMEM[0x%04X] = 0x%04X\n", addr, imem->memory[addr]);
    }
}

void dump_instruction_memory(InstructionMemory *imem) {
    printf("\nInstruction Memory Dump:\n");
    for (int i = 0; i < INSTR_MEM_SIZE; i++) {
        if (i % 8 == 0) {  // Print address every 8 instructions
            printf("\n0x%04X: ", i);
        }
        printf("%04X ", imem->memory[i]);
    }
    printf("\n");
}