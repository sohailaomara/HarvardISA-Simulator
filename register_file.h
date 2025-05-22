// register_file.h

#ifndef REGISTER_FILE_H
#define REGISTER_FILE_H

#include <stdint.h>
extern uint16_t PC;

void initPC();
void incrementPC();
void setPC(uint16_t value);
void printPC();


// 64 General-Purpose Registers
extern uint8_t registers[64];

// Function declarations
void initRegisters();
void printRegisters();
// Add to existing content
void print_register_changes(uint8_t reg_num, uint8_t old_val, uint8_t new_val);
void dump_all_registers(void);

#endif