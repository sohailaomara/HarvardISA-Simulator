// sreg.h

#ifndef SREG_H
#define SREG_H

#include <stdint.h>

typedef struct {
    uint8_t C : 1; // Carry
    uint8_t V : 1; // Overflow
    uint8_t N : 1; // Negative
    uint8_t S : 1; // Sign (N ⊕ V)
    uint8_t Z : 1; // Zero
    uint8_t reserved : 3; // Always 0
} SREG_t;

extern SREG_t SREG;

// Update functions
void updateCarry(uint16_t result);
void updateOverflow(int8_t a, int8_t b, int8_t result, char op);
void updateNegative(int8_t result);
void updateZero(uint8_t result);
void updateSign();

void printSREG();

#endif