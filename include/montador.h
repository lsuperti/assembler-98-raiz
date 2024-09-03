#ifndef MONTADOR_H
#define MONTADOR_H

#include "montador_types.h"
#include "parser.h"

void            tokenize( program_t *program );
void            assembleProgram( program_t *program, FILE *output );
void            appendSections( program_t *program, FILE *output );
void            freeProgram( program_t *program );
//              vvvv Lucas Superti, Rodrigo Santos e Gabriel Oliveira
void            parse( program_t *program );

//              vvvv Rodrigo Raupp e Fernanda Petiz
token_t         nextToken( program_t *program );

program_t*      createProgram( FILE *input );

#endif // MONTADOR_H
