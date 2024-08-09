#ifndef ARCHITECTURE_H
#define ARCHITECTURE_H

#include <stdint.h>
#include "types.h"

#define MEMORY_SIZE 1024 

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

