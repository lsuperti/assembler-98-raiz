
#include "data_structures.h"

void initVector ( Vector *a, size_t initialSize ) 
{
    a->array = malloc(initialSize * sizeof(word_t));
    a->used = 0;
    a->size = initialSize;
}

void insert( Vector *a, word_t element ) 
{
    if ( a->used == a->size )
    {
        a->size *= 1.5;
        a->array = realloc(a->array, a->size * sizeof(word_t) );
    }
    a->array[a->used++] = element;
}

void concatenate( Vector *a, Vector *b ) 
{
    for ( int i=0; i < b->used; i++ )
    {
        insert(a, b->array[i]);
    }
}

void freeVector( Vector *a )
{
    if ( a != NULL )
    {
        if ( a->array != NULL ) 
        {
            free(a->array);
            a->array = NULL;
            a->used = a->size = 0;
        }
        free(a);
        a = NULL;
    }
}

Vector *dupVector ( Vector *a )
{
    if ( a == NULL )
        return NULL;

    Vector *t;
    if ( (t = malloc(sizeof(Vector))) == NULL )
        return NULL;
    printf("hellp");

    //initVector( t, 5 );
    //for ( int i=0; i < a->used; ++i ) 
    //    insert( t, a->array[i] );

    return t;
}

