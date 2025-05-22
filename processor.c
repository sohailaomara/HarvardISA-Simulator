#include "processor.h"
#include <stdio.h>

uint32_t global_cycle = 0;

// Simplified and direct instruction name mapping
const char* opcode_to_str(uint8_t opcode) {
    static const char* mnemonics[] = {
        "ADD", "SUB", "MUL", "MOVI", "BEQZ", "ANDI",
        "EOR", "BR", "SAL", "SAR", "LDR", "STR"
    };
    return (opcode < 12) ? mnemonics[opcode] : "UNKNOWN";
}


void execute_instruction(instruction_t instr) {
    uint8_t opcode = (instr >> 12) & 0xF;
    uint8_t r1 = (instr >> 6) & 0x3F;
    uint8_t r2_or_imm = instr & 0x3F;
    
    printf("Raw instr: 0x%04X\n", instr);
    printf("Decoded: op=%d, r1=%d, r2=%d\n", opcode, r1, r2_or_imm);
    uint16_t temp_result;
    int16_t signed_temp;

    switch(opcode) {
        case 0: // ADD (R-format)
            printf("Before ADD: R%d=%d, R%d=%d\n", r1, registers[r1], r2_or_imm, registers[r2_or_imm]);
            temp_result = registers[r1] + registers[r2_or_imm];
            registers[r1] = (uint8_t)temp_result;
            printf("After ADD: R%d=%d\n", r1, registers[r1]);
            updateCarry(temp_result);
            updateOverflow(registers[r1], registers[r2_or_imm], (int8_t)registers[r1], '+');
            break;
            
        case 1: // SUB (R-format)
            temp_result = registers[r1] - registers[r2_or_imm];
            registers[r1] = (uint8_t)temp_result;
            updateOverflow(registers[r1], registers[r2_or_imm], (int8_t)registers[r1], '-');
            break;
            
        case 2: // MUL (R-format)
            temp_result = registers[r1] * registers[r2_or_imm];
            registers[r1] = (uint8_t)temp_result;
            updateCarry(temp_result);
            break;
            
        case 3: // MOVI (I-format)
            registers[r1] = r2_or_imm; // Direct 6-bit immediate
            break;
            
        case 4: // BEQZ (I-format)
            if (registers[r1] == 0) {
                PC += r2_or_imm; // Signed offset
            }
            break;
            
        case 5: // ANDI (I-format)
            registers[r1] &= r2_or_imm;
            break;
            
        case 6: // EOR (R-format)
            registers[r1] ^= registers[r2_or_imm];
            break;
            
        case 7: // BR (R-format)
            PC = ((uint16_t)registers[r1] << 6) | registers[r2_or_imm];
            return; // Skip PC increment
            
        case 8: // SAL (I-format)
            registers[r1] <<= r2_or_imm;
            updateNegative((int8_t)registers[r1]); // N
            updateZero(registers[r1]); // Z
            break;
            
        case 9: // SAR (I-format)
            signed_temp = (int8_t)registers[r1] >> r2_or_imm;
            registers[r1] = (uint8_t)signed_temp;
            updateNegative((int8_t)registers[r1]); // N
            updateZero(registers[r1]); // Z
            break;
            
        case 10: // LDR (I-format)
            registers[r1] = load_byte(r2_or_imm);
            break;
            
        case 11: // STR (I-format)
            store_byte(r2_or_imm, registers[r1]);
            break;
            
        default:
            printf("Unknown opcode: %d\n", opcode);
            break;
    }

    // Update common flags
    if (opcode <= 6) { // All arithmetic/logical ops
        updateZero(registers[r1]);
        updateNegative((int8_t)registers[r1]);
        updateSign();
    }
}

void step_cycle(InstructionMemory *imem) {
    printf("\n[Cycle %d]\n", global_cycle);

    // FETCH
    printf("FETCH:   PC=0x%04X\n", PC);
    instruction_t instr = read_instruction(imem, PC);

    // DECODE
    uint8_t opcode = (instr >> 12) & 0xF;
    uint8_t r1 = (instr >> 6) & 0x3F;
    uint8_t r2_or_imm = instr & 0x3F;
    printf("DECODE:  Raw=0x%04X | %s R%d, %d\n", instr, opcode_to_str(opcode), r1, r2_or_imm);

    uint8_t old_reg = registers[r1];
    uint8_t old_mem = 0;
    uint16_t mem_addr = 0;
    if (opcode == 11) {
        mem_addr = instr & 0x3F;
        old_mem = load_byte(mem_addr);
    }

    // EXECUTE
    printf("EXECUTE: ");
    execute_instruction(instr);

    if (opcode == 4) { // BEQZ
        if (registers[(instr >> 6) & 0x3F] == 0) {
            PC += (int8_t)(instr & 0x3F); // Signed offset
        } else {
            incrementPC();
        }
    } 
    else if (opcode != 7) { // Not BR (which already handled PC)
        incrementPC();
    }

    print_register_changes(r1, old_reg, registers[r1]);

    if (opcode == 11) {
        uint8_t new_mem = load_byte(mem_addr);
        printf("  MEM[0x%04X] changed: 0x%02X -> 0x%02X\n", mem_addr, old_mem, new_mem);
    }

    global_cycle++;
}

void run_processor(InstructionMemory *imem, uint16_t start_addr, uint16_t end_addr) {
    setPC(start_addr);

    printf("=== Processor Start ===\n");
    dump_all_registers();
    printSREG();

    while (PC <= end_addr) {
        instruction_t instr = read_instruction(imem, PC);
        if (instr == 0x0000) { // HALT or NOP: stop execution
            printf("\n[HALT] PC=0x%04X, Cycle=%u\n", PC, global_cycle);
            break;
        }
        step_cycle(imem);
    }

    printf("\n=== Processor Final State ===\n");
    dump_all_registers();
    printSREG();

    printf("\nNon-zero Data Memory:\n");
    for (int i = 0; i < DATA_MEM_SIZE; ++i) {
        if (data_memory[i] != 0) {
            printf("  MEM[0x%04X] = 0x%02X\n", i, data_memory[i]);
        }
    }

    printf("\nInstruction Memory (non-zero):\n");
    for (int i = 0; i < INSTR_MEM_SIZE; ++i) {
        if (imem->memory[i] != 0) {
            printf("  0x%04X: %s R%d, %d\n", i, opcode_to_str(imem->memory[i] >> 12),
                   (imem->memory[i] >> 6) & 0x3F, imem->memory[i] & 0x3F);
        }
    }
}

// Pipeline stage implementations
void pipeline_fetch(InstructionMemory *imem, PipelineStage *stage) {
    stage->instruction = read_instruction(imem, PC);
    stage->pc = PC;
    stage->valid = (stage->instruction != 0x0000); // Valid if not HALT/NOP
    incrementPC();
    
    printf("IF Stage:\n");
    printf("  PC: 0x%04X\n", stage->pc);
    printf("  Instruction: 0x%04X\n", stage->instruction);
    if (stage->valid) {
        uint8_t opcode = (stage->instruction >> 12) & 0xF;
        uint8_t r1 = (stage->instruction >> 6) & 0x3F;
        uint8_t r2_or_imm = stage->instruction & 0x3F;
        printf("  Decoded: %s R%d, %d\n", opcode_to_str(opcode), r1, r2_or_imm);
    } else {
        printf("  HALT/NOP detected\n");
    }
}

void pipeline_decode(PipelineStage *stage) {
    if (!stage->valid) {
        printf("ID Stage: Empty\n");
        return;
    }
    
    stage->opcode = (stage->instruction >> 12) & 0xF;
    stage->r1 = (stage->instruction >> 6) & 0x3F;
    stage->r2_or_imm = stage->instruction & 0x3F;
    
    // Save old register value for change detection
    stage->old_reg_value = registers[stage->r1];
    
    // Save old memory value for STR instructions
    if (stage->opcode == 11) { // STR
        stage->mem_addr = stage->instruction & 0x3F;
        stage->old_mem_value = load_byte(stage->mem_addr);
    }
    
    printf("ID Stage:\n");
    printf("  Instruction: 0x%04X\n", stage->instruction);
    printf("  Operation: %s\n", opcode_to_str(stage->opcode));
    printf("  Parameters: R%d, %d\n", stage->r1, stage->r2_or_imm);
    printf("  Current Register Value: R%d = 0x%02X\n", stage->r1, stage->old_reg_value);
    
    if (stage->opcode == 11) { // STR
        printf("  Memory Address: 0x%04X\n", stage->mem_addr);
        printf("  Current Memory Value: 0x%02X\n", stage->old_mem_value);
    }
}

void pipeline_execute(PipelineStage *stage) {
    if (!stage->valid) {
        printf("EX Stage: Empty\n");
        return;
    }
    
    printf("EX Stage:\n");
    printf("  Instruction: 0x%04X\n", stage->instruction);
    printf("  Operation: %s\n", opcode_to_str(stage->opcode));
    printf("  Parameters: R%d, %d\n", stage->r1, stage->r2_or_imm);
    
    // Execute the instruction
    execute_instruction(stage->instruction);
    
    // Print register changes
    if (registers[stage->r1] != stage->old_reg_value) {
        printf("  Register Update: R%d = 0x%02X -> 0x%02X\n", 
               stage->r1, stage->old_reg_value, registers[stage->r1]);
    }
    
    // Print memory changes for STR
    if (stage->opcode == 11) {
        uint8_t new_mem = load_byte(stage->mem_addr);
        if (new_mem != stage->old_mem_value) {
            printf("  Memory Update: MEM[0x%04X] = 0x%02X -> 0x%02X\n", 
                   stage->mem_addr, stage->old_mem_value, new_mem);
        }
    }
}

void run_pipelined_processor(InstructionMemory *imem, uint16_t start_addr, uint16_t end_addr) {
    setPC(start_addr);
    global_cycle = 1;  // Start from 1 instead of 0
    
    printf("=== Processor Start ===\n");
    printf("Initial Register State:\n");
    dump_all_registers();
    printf("\nInitial Status Register State:\n");
    printSREG();
    
    // Initialize pipeline stages
    PipelineStage fetch_stage = {0};
    PipelineStage decode_stage = {0};
    PipelineStage execute_stage = {0};
    
    bool running = true;
    while (running) {
        printf("\n=== Clock Cycle %d ===\n", global_cycle);
        
        // Fetch stage (newest instruction)
        pipeline_fetch(imem, &fetch_stage);
        
        // Decode stage (middle instruction)
        pipeline_decode(&decode_stage);
        
        // Execute stage (oldest instruction)
        pipeline_execute(&execute_stage);
        
        // Move instructions through pipeline
        execute_stage = decode_stage;
        decode_stage = fetch_stage;
        fetch_stage.valid = false;
        
        // Check for halt condition
        if (!fetch_stage.valid && !decode_stage.valid && !execute_stage.valid) {
            running = false;
        }
        
        // Check if we've reached the end address
        if (PC > end_addr) {
            running = false;
        }
        
        global_cycle++;
    }
    
    printf("\n=== Final Processor State ===\n");
    printf("\nRegister Contents:\n");
    dump_all_registers();
    printf("\nStatus Register State:\n");
    printSREG();
    
    printf("\nData Memory Contents:\n");
    for (int i = 0; i < DATA_MEM_SIZE; ++i) {
        if (i % 16 == 0) {
            printf("\n0x%04X: ", i);
        }
        printf("%02X ", data_memory[i]);
    }
    printf("\n");
    
    printf("\nFinal Program Counter (PC): 0x%04X\n", PC);

    printf("\nInstruction Memory Contents:\n");
    for (int i = 0; i < INSTR_MEM_SIZE; ++i) {
        if (i % 8 == 0) {
            printf("\n0x%04X: ", i);
        }
        printf("%04X ", imem->memory[i]);
    }
    printf("\n");
}