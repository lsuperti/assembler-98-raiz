
#include "architecture.h"

// 16kB of memory.
int16_t memory[MEMORY_SIZE];

// Registers
// Stack size = 4kB
// Data region size = 4kB
// Instruction region size = 8kB
int16_t accumulator;
int16_t program_counter = 256;
int8_t  mop = 0;
int16_t ri = 0;
int16_t data_reg = 767;

