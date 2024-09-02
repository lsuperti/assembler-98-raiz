
#include "data_structures.h"

void initVector ( Vector *a, size_t initialSize ) 
{
    a->array = malloc(initialSize * sizeof(int));
    assert(a->array != NULL );
    a->used = 0;
    a->size = initialSize;
}

void insert( Vector *a, int element ) 
{
    if ( a->used == a->size )
    {
        a->size *= 1.5;
        a->array = realloc(a->array, a->size * sizeof(int) );
        assert( a->array != NULL );
    }
    a->array[a->used++] = element;
}

void freeVector( Vector *a )
{
    free(a->array);
    a->array = NULL;
    a->used = a->size = 0;
}
