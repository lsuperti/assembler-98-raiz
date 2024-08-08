
#include "architecture.h"
#include "types.h"

// 16kB of memory.
word_t memory[MEMORY_SIZE];

// Registers
// Stack size = 4kB
// Data region size = 4kB
// Instruction region size = 8kB
word_t accumulator;
word_t program_counter = 256;
int8_t  mop = -1;
word_t ri;
word_t data_reg = 767;

