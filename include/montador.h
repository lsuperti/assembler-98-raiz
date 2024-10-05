#ifndef MONTADOR_H
#define MONTADOR_H

#include "montador_types.h"
#include "parser.h"
#include "uthash.h"
#include "macro_processor.h"

void            tokenize( program_t *program );
program_t       *assembleProgram( char *file_path, char *output_path );
void            generateOutput( program_t *program, FILE *output );
void            printTokens( program_t *program );
void            parse( program_t *program );

token_t         nextToken( program_t *program );

#endif // MONTADOR_H
