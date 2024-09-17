
#include "macro_processor.h"
#include "montador.h"

void addToken( token_t **tokens, int *capacity, int *idx, token_t tok )
{
    if (*idx >= *capacity) {
        *capacity *= 2;
        *tokens = (token_t *) realloc(*tokens, sizeof(token_t) * (*capacity));
        assert(*tokens != NULL);
    }

    (*tokens)[*idx] = tok;
    (*idx)++;
}

void tokenizeMacro( program_t *program, MACRO_T *m )
{
    int capacity = 10;
    token_t *tokens = (token_t *) malloc(sizeof(token_t) * capacity);
    assert(tokens != NULL);

    token_t tok;
    int idx = 0;

    do { 
        program->token_idx++;
        tok = program->tokens[program->token_idx];
        
        if (tok.type == TOK_MACRO_END) {
            break;
        }

        addToken(&tokens, &capacity, &idx, tok);
        //printf("%s: %d:%d\n", tok.token, tok.line, tok.column);
    } while (true);

    if (idx > 0) {
        tokens = (token_t *) realloc(tokens, sizeof(token_t) * idx);
        assert(tokens != NULL);
    } else {
        free(tokens);

        tokens = NULL;
        idx = 0;
    }

    m->tokens = tokens;
    m->n_tokens = idx;
}

void defineMode( program_t *program )
{
    MACRO_T *m = malloc( sizeof( MACRO_T ) ); 
    tokenizeMacro(program, m);
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

