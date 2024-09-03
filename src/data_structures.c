
#include "data_structures.h"

void initVector ( Vector *a, size_t initialSize ) 
{
    a->array = malloc(initialSize * sizeof(word_t));
    a->used = 0;
    a->size = initialSize;
}

void insert( Vector *a, int element ) 
{
    if ( a->used == a->size )
    {
        a->size *= 1.5;
        a->array = realloc(a->array, a->size * sizeof(word_t) );
    }
    a->array[a->used++] = element;
}

void freeVector( Vector *a )
{
    if ( a->array ) 
    {
        free(a->array);
        a->array = NULL;
        a->used = a->size = 0;
    }
    free(a);
    a = NULL;
}

