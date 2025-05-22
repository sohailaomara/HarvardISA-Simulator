// register_file.c

#include "register_file.h"
#include <stdio.h>

uint8_t registers[64];  // Define the actual register array
uint16_t PC = 0;

void initPC() {
    PC = 0;
}

void incrementPC() {
    PC++;
}

void setPC(uint16_t value) {
    PC = value;
}

void printPC() {
    printf("PC = 0x%04X (%u)\n", PC, PC);
}


void initRegisters() {
    for (int i = 0; i < 64; i++) {
        registers[i] = 0;
    }
}

void printRegisters() {
    for (int i = 0; i < 64; i++) {
        printf("R%d = %u\n", i, registers[i]);
  }
}

// Add after existing functions
void print_register_changes(uint8_t reg_num, uint8_t old_val, uint8_t new_val) {
    if (old_val != new_val) {
        printf("  R%d changed: 0x%02X -> 0x%02X\n", reg_num, old_val, new_val);
    }
}

void dump_all_registers() {
    printf("\nRegister Listing:\n");
    for (int i = 0; i < 64; i += 8) {
        for (int j = 0; j < 8; j++) {
            if (i+j < 64) printf("R%-2d=0x%02X ", i+j, registers[i+j]);
        }
        printf("\n");
    }
}