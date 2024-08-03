#ifndef ARCHITECTURE_H
#define ARCHITECTURE_H

#include <stdint.h>

#define MEMORY_SIZE 1024 

// 16kB of memory.
extern int16_t memory[MEMORY_SIZE];

// Registers
// Stack size = 4kB
// Data region size = 4kB
// Instruction region size = 8kB
extern int16_t accumulator;
extern int16_t program_counter;
extern int8_t  mop;
extern int16_t ri;
extern int16_t data_reg;

#endif // ARCHITECTURE_H

