#ifndef INSTRUCTION_MEMORY_H
#define INSTRUCTION_MEMORY_H

#include <stdio.h>
#include <stdint.h>

#define INSTR_MEM_SIZE 1024      // 2^10 = 1024 words
#define INSTR_WIDTH 16           // 16 bits per word
typedef uint16_t instruction_t;  // 16-bit instruction type

// Instruction Memory structure
typedef struct {
    instruction_t memory[INSTR_MEM_SIZE]; // 1024 x 16-bit memory
} InstructionMemory;

// Function prototypes
void init_instruction_memory(InstructionMemory *imem);
instruction_t read_instruction(InstructionMemory *imem, uint16_t address);
void write_instruction(InstructionMemory *imem, uint16_t address, instruction_t instruction);
void print_instruction_memory(InstructionMemory *imem, uint16_t start_addr, uint16_t end_addr);
void dump_instruction_memory(InstructionMemory *imem);
// Global instruction memory instance
extern InstructionMemory imem;

#endif // INSTRUCTION_MEMORY_H