// sreg.c

#include "sreg.h"
#include <stdio.h>

SREG_t SREG = {0};

void updateCarry(uint16_t result) {
    SREG.C = (result >> 8) & 1;
}

void updateOverflow(int8_t a, int8_t b, int8_t result, char op) {
    if (op == '+') {
        SREG.V = ((a > 0 && b > 0 && result < 0) || (a < 0 && b < 0 && result > 0));
    } else if (op == '-') {
        SREG.V = ((a > 0 && b < 0 && result < 0) || (a < 0 && b > 0 && result > 0));
    }
}

void updateNegative(int8_t result) {
    SREG.N = (result < 0);
}

void updateZero(uint8_t result) {
    SREG.Z = (result == 0);
}

void updateSign() {
    SREG.S = SREG.N ^ SREG.V;
}

void printSREG() {
    printf("SREG: C=%d V=%d N=%d S=%d Z=%d\n", SREG.C, SREG.V, SREG.N, SREG.S, SREG.Z);
}