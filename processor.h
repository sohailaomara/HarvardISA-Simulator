// processor.h
#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <stdbool.h>
#include "instruction_memory.h"
#include "register_file.h"
#include "data_memory.h"
#include "sreg.h"

// Pipeline stage structures
typedef struct {
    instruction_t instruction;
    uint16_t pc;
    uint8_t opcode;
    uint8_t r1;
    uint8_t r2_or_imm;
    uint8_t old_reg_value;
    uint8_t old_mem_value;
    uint16_t mem_addr;
    bool valid;
} PipelineStage;

// Function prototypes
void execute_instruction(instruction_t instr);
void run_processor(InstructionMemory *imem, uint16_t start_addr, uint16_t end_addr);
void print_instruction(instruction_t instr);
extern uint32_t global_cycle;

// Pipeline functions
void pipeline_fetch(InstructionMemory *imem, PipelineStage *stage);
void pipeline_decode(PipelineStage *stage);
void pipeline_execute(PipelineStage *stage);
void run_pipelined_processor(InstructionMemory *imem, uint16_t start_addr, uint16_t end_addr);

#endif // PROCESSOR_H