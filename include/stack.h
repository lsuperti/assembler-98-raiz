#ifndef STACK_H
#define STACK_H

#include <stdbool.h>
#include "architecture.h"

extern word_t stack_pointer;

// Returns true if action was successful
// otherwise false.
bool push ( int value );

// Returns 0 if there is no value to be
// popped otherwise returns the value.
int pop ( void );
    
#endif // STACK_H
       
