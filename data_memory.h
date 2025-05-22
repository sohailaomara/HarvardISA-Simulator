#include <stdint.h>
#define DATA_MEM_SIZE 2048
extern uint8_t data_memory[DATA_MEM_SIZE];
void init_data_memory();
uint8_t load_byte(uint16_t address);
void store_byte(uint16_t address, uint8_t value);
void print_data_memory();