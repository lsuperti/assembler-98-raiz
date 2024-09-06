#ifndef MONTADOR_H
#define MONTADOR_H

#include "montador_types.h"
#include "parser.h"

void            tokenize( program_t *program );
program_t       *assembleProgram( char *file_path, char *output_path );
void            generateOutput( program_t *program, FILE *output );
void            freeProgram( program_t *program );
void            printTokens( program_t *program );
void            parse( program_t *program );

token_t         nextToken( program_t *program );

program_t*      createProgram( FILE *input );

#endif // MONTADOR_H
