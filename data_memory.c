#include <stdint.h>
#include <stdio.h>
#include "data_memory.h"
#define DATA_MEM_SIZE 2048
uint8_t data_memory[DATA_MEM_SIZE];

void init_data_memory() {
    for (int i = 0; i < DATA_MEM_SIZE; i++) {
        data_memory[i] = 0;
    }
}

uint8_t load_byte(uint16_t address) {
    if (address >= DATA_MEM_SIZE) {
        return 0;
    }
    return data_memory[address];
}

void store_byte(uint16_t address, uint8_t value) {
    if (address >= DATA_MEM_SIZE) {
        return;
    }
    data_memory[address] = value;
    
    printf("MEM[%d] = %d (0x%02X)\n", address, value, value);
}

void print_data_memory() {
    printf("\nData Memory Contents:\n");
    for (int i = 0; i < DATA_MEM_SIZE; i++) {
        if (i % 16 == 0) {
            printf("\n0x%04X: ", i);
        }
        printf("%02X ", data_memory[i]);
    }
    printf("\n");
}