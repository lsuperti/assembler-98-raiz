#ifndef MONTADOR_H
#define MONTADOR_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "global.h"

enum s_types {
    EXECUTABLE = 1,
    DATA = 2,
    S_TYPES_SIZE
};

enum t_types {
    OPCODE = 1,
    OPERAND_1 = 2,
    OPERAND_2 = 3,
    T_TYPES_SIZE
};

typedef struct _token_t {
    char  *token;
    enum   t_types type;
    int    value;
    size_t size;
} token_t;

typedef struct _section_t {
    char    *section_content;
    char    *section_title;
    enum     s_types section_type; 
    size_t   section_size;
    int      head;
    token_t *tokens;
} section_t;

typedef struct _symbol_table_t {
    token_t *tokens;
    size_t   num_s;
} symbol_table_t;

typedef struct _program_t{
    section_t     *sections;
    symbol_table_t symbols;
    size_t         program_size;
    char          *elf98;
} program_t;

symbol_table_t* generateSTable( section_t *dot_text, section_t *dot_data );
program_t*      createProgram( FILE *file );
token_t*        nextToken( section_t *section );
section_t*      getSection( FILE *file, const char *section_title );

#endif // MONTADOR_H
