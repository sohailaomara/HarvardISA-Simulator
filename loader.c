#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "instruction_memory.h"

// Map mnemonic to opcode (0–11)
int get_opcode(const char *mnemonic) {
    const char *mnemonics[] = {
        "ADD", "SUB", "MUL", "MOVI", "BEQZ", "ANDI",
        "EOR", "BR", "SAL", "SAR", "LDR", "STR"
    };
    for (int i = 0; i < 12; i++) {
        if (strcmp(mnemonic, mnemonics[i]) == 0) return i;
    }
    return -1;
}

// Check if instruction is I-format
int is_iformat(int opcode) {
    return opcode == 3 || opcode == 4 || opcode == 5 ||
           opcode == 8 || opcode == 9 || opcode == 10 || opcode == 11;
}

// Load and encode program
int load_program(const char *filename, InstructionMemory *imem) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Cannot open file %s\n", filename);
        return -1;
    }

    char line[128];
    uint16_t addr = 0;
    while (fgets(line, sizeof(line), file)) {
        // Ignore comments
        char *semicolon = strchr(line, ';');
        if (semicolon) *semicolon = '\0';

        char mnemonic[8];
        if (sscanf(line, "%7s", mnemonic) != 1) continue;

        int opcode = get_opcode(mnemonic);
        if (opcode < 0) continue;

        uint16_t instr = (opcode << 12); // opcode in bits 15-12

        // I-format
        if (is_iformat(opcode)) {
            int rd, imm;
            if (sscanf(line, "%*s R%d %d", &rd, &imm) == 2) {
                instr |= ((rd & 0x3F) << 6);           // Rd (bits 11-6)
                instr |= (imm & 0x3F);                 // Immediate (bits 5-0)
            }
        }
        // BR: special case – R1 and R2
        else if (opcode == 7) { // BR
            int r1, r2;
            if (sscanf(line, "%*s R%d R%d", &r1, &r2) == 2) {
                instr |= ((r1 & 0x3F) << 6);           // R1 (bits 11-6)
                instr |= (r2 & 0x3F);                  // R2 (bits 5-0)
            }
        }
        // R-format (default case)
        else {
            int rd, rs;
            if (sscanf(line, "%*s R%d R%d", &rd, &rs) == 2) {
                instr |= ((rd & 0x3F) << 6);           // Rd (bits 11-6)
                instr |= (rs & 0x3F);                  // Rs (bits 5-0)
            }
        }

        // Store instruction in memory
        write_instruction(imem, addr++, instr);
    }

    fclose(file);
    return 0;
}
