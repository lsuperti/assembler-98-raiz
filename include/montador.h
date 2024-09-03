#ifndef MONTADOR_H
#define MONTADOR_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "global.h"
#include "architecture.h"
#include "data_structures.h"

// Lista de tipos de tokens.
enum t_types {
    TOK_INSTRUCTION  = 1,
    TOK_IDENTIFIER   = 4,
    TOK_SECTION      = 5,
    TOK_ADDRESSING   = 6,
    TOK_LITERAL      = 7,
    TOK_LITERAL_HEX  = 902,
    TOK_STACK        = 22,
    TOK_COPY         = 222,
    TOK_RET          = 402,
    TOK_STOP         = 502,
    TOK_LABEL        = 702,
    TOK_SECTION_NAME = 802,
    TOK_NT_STRING    = 822,
    TOK_WORD         = 823,
    TOK_ASCIIZ       = 824,
    TOK_UNKNOWN      = 825,
    TOK_GLOBAL       = 826,
    TOK_EXTERN       = 827,
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
    Vector *dot_text;
    Vector *dot_data;
    Vector *dot_rodata;
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
