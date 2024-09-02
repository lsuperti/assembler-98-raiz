#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <types.h>
#include <assert.h>

// Vetor dinâmico com fator de crescimento 1.5X 
// https://en.wikipedia.org/wiki/Dynamic_array
typedef struct _Vector {
    int *array;
    size_t used;
    size_t size;
} Vector;

void initVector( Vector *a, size_t initialSize );
void insert( Vector *a, int element );
void freeVector( Vector *a );

#endif  // DATA_STRUCTURES_H

