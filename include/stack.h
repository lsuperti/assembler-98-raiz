#ifndef STACK_H
#define STACK_H

#include <stdint.h>
#include <gtk/gtk.h>
#include <stdbool.h>
#include "architecture.h"

int16_t stack_pointer = 255;

// Returns true if action was successful
// otherwise false.
bool push ( int value ) {
    if ( stack_pointer - 1 < 0 ) {
        return 0;
    }
    memory[ stack_pointer-- ] = value;
    return 1;
}

// Returns 0 if there is no value to be
// popped otherwise returns the value.
int pop ( void ) { 
    if ( 255 == stack_pointer ) {
        return 0;
    }
    return memory[ ++stack_pointer ];
}
    
#endif // STACK_H
       
