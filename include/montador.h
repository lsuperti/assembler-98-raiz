#ifndef MONTADOR_H
#define MONTADOR_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "global.h"
#include "architecture.h"

enum t_types {
    INSTRUCTION = 1,
    OPERAND_1   = 2,
    OPERAND_2   = 3,
    IDENTIFIER  = 4,
    SECTION     = 5,
    ADDRESSING  = 6,
    LITERAL     = 7,
    T_TYPES_SIZE
};

/* 
 * Valor = 0 se for uma keyword como section.
 * LOAD 20   -> LOAD_IMMEDIATE 20
 * Exemplo_1 > 
 * char  *token       = "LOAD",
 * enum   t_types type = INSTRUCTION,
 * bool   defined      = false,
 * word_t value        = X,
 *
 * O valor depende do proximo token então
 * so pode ser colocado na hora do parsing
 * por exemplo LOAD pode ser ( 3, 50, 51 ).
 *
 * Exemplo_2 > 
 * char   *token       = "ZERO",
 * enum   t_types type = IDENTIFIER,
 * bool   defined      = false,
 * word_t value        = X
*/
typedef struct _token_t {
    char   *token;
    enum   t_types type;
    bool   defined;
    word_t value;
} token_t;

/*
 * Tabela de símbolos ou tokens 
 * com type = IDENTIFIER.
*/
typedef struct _symbol_table_t {
    token_t *tokens;
    size_t   num_s;
} symbol_table_t;

/*
 * Seções do arquivo binario
 * exemplo em program.elf98
*/ 
typedef struct _sections_t {
    word_t *dot_text;
    word_t *dot_data;
    word_t *dot_rodata;
} sections_t;

/*
 * source       = conteudo de um arquivo target { Ex : syntax }
 * sections     = vetores com os dados das seções suportadas 
 *                { .text, .data, .rodata }
 * tokens       = todos os tokens do arquivo target começa com nenhum.
 * table        = tokens que são identifiers começa com nenhum.
 * head         = cabeçote do atual caracter do source
 * program_size = strlen( source )
*/

typedef struct _program_t{
    char           *source;
    sections_t     *sections;
    token_t        *tokens;
    size_t          n_tokens;
    symbol_table_t *table;
    int             HEAD;
    size_t          program_size;
} program_t;

void            tokenize( program_t *program );
void            assembleProgram( program_t *program, FILE *output );
void            appendSections( program_t *program, FILE *output );
void            freeProgram( program_t *program );

//              vvvv Lucas Superti, Rodrigo Santos e Gabriel Oliveira
symbol_table_t* parse( program_t *program );

//              vvvv Rodrigo Raupp e Fernanda Petiz
token_t         nextToken( program_t *program );

program_t*      createProgram( FILE *input );

#endif // MONTADOR_H
