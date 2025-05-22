#include <stdio.h>
#include "data_memory.h"
#include "register_file.h"
#include "sreg.h"
#include "instruction_memory.h"
#include "processor.h"
#include "loader.c" 

int main() {
    // Initialize everything
    InstructionMemory imem;
    init_instruction_memory(&imem);
    initRegisters();
    init_data_memory();

    data_memory[20] = 42;
    data_memory[21] = 99;
    
    initPC();
    global_cycle = 0;
    
    // Load program
    if (load_program("test.txt", &imem) != 0) {
        fprintf(stderr, "Failed to load program\n");
        return 1;
    }
    print_instruction_memory(&imem, 0, 10);
    printf("=== Data Memory BEFORE Execution ===\n");
    print_data_memory();
    
    printf("=== Starting Pipelined Execution ===\n");
    
    // Use pipelined processor
    run_pipelined_processor(&imem, 0, 10);
    
    return 0;
}