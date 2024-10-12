#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include "types.h"
#include <assert.h>
#include <stdint.h>

// Vetor dinâmico com fator de crescimento 1.5X 
// https://en.wikipedia.org/wiki/Dynamic_array
typedef struct _Vector {
    word_t *array;
    size_t used;
    size_t size;
} Vector;

typedef struct _GenericVector{
    void  *array;
    size_t used;
    size_t size;
} GenericVector;

void    concatenate( Vector *a, Vector *b );
void    initVector( Vector *a, size_t initialSize );
void    insert    ( Vector *a, word_t element );
void    freeVector( Vector *a );
Vector *dupVector ( Vector *a );

#endif  // DATA_STRUCTURES_H

