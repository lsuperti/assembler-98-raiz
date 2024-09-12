
#include "macro_processor.h"

void defineMode( program_t *program )
{
    MACRO_T *m = malloc( sizeof( MACRO_T ) ); 

}

// Expand mode for program->tokens
// Which will be different than the one used 
// when on defineMode.
void expandMode( program_t *program )
{
}

void process_macros( program_t *program )
{
    program->token_idx = 0;
    token_t tok;
    while ( program->token_idx < program->n_tokens )
    {
        tok = program->tokens[program->token_idx];
        switch ( tok.type )
        {
            case TOK_MACRO_START:
                defineMode(program);
            break;
            case TOK_IDENTIFIER:
                if ( find_macro( program, tok.token ) != NULL )
                    expandMode(program);
            break;
            default:
                program->token_idx++;
            break;
        }
    }
}

void add_macro( program_t *program, MACRO_T *macro ) 
{
    HASH_ADD_STR( program->macros, name, macro );
}

MACRO_T *find_macro( program_t *program, char *name )
{
    MACRO_T *m;
    HASH_FIND_STR( program->macros, name, m);
    return m;
}

