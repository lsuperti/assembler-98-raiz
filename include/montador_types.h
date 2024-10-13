#ifndef MONTADOR_TYPES_H
#define MONTADOR_TYPES_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "global.h"
#include "architecture.h"
#include "uthash.h"
#include "data_structures.h"

// Lista de tipos de tokens.
enum t_types {
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
    TOK_STRING       = 822, //*  Não feito ainda no nextToken
    TOK_WORD         = 823,
    TOK_ASCIIZ       = 824, //*  Não feito ainda no nextToken
    TOK_UNKNOWN      = 825,
    TOK_GLOBAL       = 826,
    TOK_EXTERN       = 827,
    TOK_LOAD         = 1001,
    TOK_ADD          = 1002,
    TOK_BR           = 1003,
    TOK_BRNEG        = 1004,
    TOK_BRPOS        = 1005,
    TOK_BRZERO       = 1006,
    TOK_CALL         = 1007,
    TOK_DIVIDE       = 1008,
    TOK_MULT         = 1009,
    TOK_READ         = 1010,
    TOK_STORE        = 1011,
    TOK_SUB          = 1012,
    TOK_WRITE        = 1013,
    TOK_PUT          = 1014,
    TOK_SPACE        = 1015,
    TOK_NEWLINE      = 1016,
    TOK_EOF          = 1020,
    TOK_MACRO_START  = 1220,
    TOK_LOCAL_LABEL  = 1330,
    TOK_LOCAL_IDENTIFIER = 1350,
    TOK_MACRO_END    = 1230,
    TOK_BLANK        = 1632,
    TOK_PARAM_DELIMITER = 1743,
    T_TYPES_SIZE
};

extern const char * const tok_colors[];

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
    char   *token;          //texto do token encontrado em source
    char   *formal;         
    enum   t_types type;    //o tipo do token
    bool   defined;         //se token esta definido ou nao
    word_t value;           //valor literal ou posicao de memoria
    int    offset;
    int    line;
    int    column;
    Vector pos;
    bool   data_l;
    UT_hash_handle hh;
} token_t;

/*
 * Definição de Macro
*/ 
typedef struct _MACRO_T {
    char    *name;
    token_t *tokens;
    size_t   n_tokens; 
    token_t *params;
    size_t   n_params;
    struct _MACRO_T *local_macros;
    size_t   n_local_macros;
    size_t   called;
    UT_hash_handle hh;
} MACRO_T ;

/*
 * Tabela de símbolos ou tokens 
 * com type = IDENTIFIER.
*/
typedef struct _symbol_table_t {
    token_t *tokens;        //vetor de tokens (simbolos) na tabela
    size_t   num_s;         //numero total de simbolos na tabela
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

/**
 * source       = conteudo de um arquivo target { Ex : syntax }
 * sections     = vetores com os dados das seções suportadas 
 *                { .text, .data, .rodata }
 * tokens       = todos os tokens do arquivo target começa com nenhum.
 * table        = tokens que são identifiers começa com nenhum.
 * head         = cabeçote do atual caracter do source
 * program_size = strlen( source )
*/
typedef struct _program_t{
  // vvvv Tokenização
    char           *source;         //string contendo codigo fonte
    sections_t     *sections;       //seções do programa
    token_t        *tokens;         //vetor contendo tokens 
    size_t          n_tokens;       //total de tokens do programa
    size_t          token_idx;      //cabecote do vetor de tokens
    size_t          n_symbols;
    int             HEAD;           //cabecote do texto em source
    size_t          program_size;
    int             c_row;
    int             c_col;
  // vvvv Resolução dos identificadores
    symbol_table_t *table;
  // vvvv Processador de macros 
    MACRO_T        *macros;
    size_t          n_macros;
    token_t        *cur_macro_params;
  // vvvv Necessário para o ligador 
    token_t        *globals;
    size_t          n_globals;
    token_t        *externs;
    size_t          n_externs;
} program_t;

token_t*        getNextToken( program_t *program );
program_t*      createProgram( FILE *input );
void            freeProgram( program_t *program );
void            tokenize( program_t *program );

#endif //MONTADOR_TYPES_H
