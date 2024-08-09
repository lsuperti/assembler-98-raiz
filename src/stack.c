#include "stack.h"
#include <stdbool.h>
#include "architecture.h"

word_t stack_pointer = 255;

bool push ( int value ) {
    if ( stack_pointer - 1 < 0 ) {
        return 0;
    }
    memory[ stack_pointer-- ] = value;
    return 1;
}

int pop ( void ) { 
    if ( 255 == stack_pointer ) {
        return 0;
    }
    return memory[ ++stack_pointer ];
}