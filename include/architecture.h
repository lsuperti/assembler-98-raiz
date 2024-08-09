#ifndef ARCHITECTURE_H
#define ARCHITECTURE_H

#include <stdint.h>
#include "types.h"

#define MEMORY_SIZE 1024 

enum instructions_t {
    LOAD_DIRECT = 3,
    LOAD_INDIRECT = 50,
    LOAD_IMMEDIATE = 51,
    COPY_DD = 13,
    COPY_DI = 71,
    COPY_DIm = 72,
    COPY_ID = 73,
    COPY_II = 74,
    COPY_IIm = 75,
    ADD_DIRECT = 2,
    ADD_INDIRECT = 52,
    ADD_IMMEDIATE = 53,
    STORE_DIRECT = 7,
    STORE_INDIRECT = 54,
    READ_DIRECT = 12,
    READ_INDIRECT = 55,
    STOP = 11,
    BRZERO_DIRECT = 4,
    BRZERO_INDIRECT = 56,
    BRZERO_IMMEDIATE = 76,
    BRPOS_DIRECT = 1,
    BRPOS_INDIRECT = 57,
    BRPOS_IMMEDIATE = 77,
    BRNEG_DIRECT = 5,
    BRNEG_INDIRECT = 58,
    BRNEG_IMMEDIATE = 78,
    WRITE_DIRECT = 8,
    WRITE_INDIRECT = 59,
    MULT_DIRECT = 14,
    MULT_INDIRECT = 60,
    MULT_IMMEDIATE = 61,
    SUB_DIRECT = 6,
    SUB_INDIRECT = 62,
    SUB_IMMEDIATE = 63,
    DIV_DIRECT = 10,
    DIV_INDIRECT = 64,
    DIV_IMMEDIATE = 65,
    BR_DIRECT = 00,
    BR_INDIRECT = 66,
    BR_IMMEDIATE = 67,
    CALL_DIRECT = 15,
    CALL_INDIRECT = 68,
    CALL_IMMEDIATE = 79,
    PUT_DIRECT = 20,
    PUT_INDIRECT = 69,
    PUT_IMMEDIATE = 70,
    RET = 16,
    PC  = 100,
    RI  = 101,
    SP  = 102,
    UNKNOWN = 103
};

// 16kB of memory.
extern word_t memory[MEMORY_SIZE];

// Registers
// Stack size = 4kB
// Data region size = 4kB
// Instruction region size = 8kB
extern word_t accumulator;
extern word_t program_counter;
extern int8_t  mop;
extern word_t ri;
extern word_t data_reg;

#endif // ARCHITECTURE_H

