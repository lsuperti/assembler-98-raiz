
#include "montador.h"
#include <ctype.h>

#define WHITESPACES " \n\t\r"

/* 
 * Arquivo com código source é passado para 
 * essa função.
*/
program_t* createProgram( FILE *file )
{ 
    assert( file != NULL );
    program_t *program =
    ( program_t * ) malloc( sizeof ( program_t ) );

    long lSize;
    char *buffer;

    fseek( file, 0L , SEEK_END);
    lSize = ftell( file );
    rewind( file );

    buffer = calloc( 1, lSize+1 );
    if( !buffer ) fclose(file),fputs("Memory alloc fails",stderr),exit(1);

    if( 1!=fread( buffer, lSize, 1, file) )
      fclose(file),free(buffer),fputs("Entire read fails",stderr),exit(1);

    program->source       = buffer;
    program->HEAD         = 0;             
    program->tokens       = NULL;
    program->sections     = NULL;
    program->table        = NULL;
    program->program_size = strlen(program->source);

    return program;
}

/*
 * Libera a memoria da struct program_t.
*/
void freeProgram( program_t *program )
{
   if ( program->source )
       free(program->source);
   if ( program->sections ) {
       if ( program->sections->dot_text )
           free(program->sections->dot_text);
       if ( program->sections->dot_data )
           free(program->sections->dot_data);
       if ( program->sections->dot_rodata)
           free(program->sections->dot_rodata);
       free(program->sections);
       program->sections = NULL;
   }
   if ( program->table ) {
       if ( program->table->tokens )
           free( program->table->tokens );
       free(program->table);
       program->table = NULL;
   }
   if ( program->tokens ) {
       if ( program->tokens->token ) 
           free(program->tokens->token);
       free(program->tokens);
       program->tokens = NULL;
   }
   free(program);
   program = NULL;
}

token_t* nextToken( program_t *program ) { 
    // Pula espaços em branco
    while ( program->HEAD < program->program_size && 
            strchr(WHITESPACES, program->source[program->HEAD]) ) {
        program->HEAD++;
    }

    // Verifica se chegou no fim
    if ( program->HEAD >= program->program_size ) {
        return NULL;
    }
    
    return (void*)0;
}

