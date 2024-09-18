
#include "macro_processor.h"
#include <errno.h>
#include "montador.h"

error printMacros( program_t *p ) 
{
    MACRO_T *entry, *tmp; 

    program_t *dirty_hack;
    if ( ( dirty_hack = malloc(sizeof(program_t)) ) == NULL )
        return ENOMEM;

    HASH_ITER(hh, p->macros, entry, tmp)
    {
        if (entry)
        {
            fprintf(stdout, "\nMACRO : %s, NUM_TOKENS : %lu, NUM_PARAMS : %lu\n",  entry->name,
                    entry->n_tokens, entry->n_params   );
            fflush(stdout);
            dirty_hack->tokens   = entry->tokens;
            dirty_hack->n_tokens = entry->n_tokens;
            printTokens( dirty_hack );
        }
    }

    free(dirty_hack);
    return EXIT_SUCCESS;
}


void goToMacroEnd( program_t *program )
{
    token_t tok;

    program->token_idx++;
    if ( program->token_idx < program->n_tokens ) 
    {
        tok = program->tokens[program->token_idx];
    }else 
    {
        return;
    }

    if (tok.type != TOK_MACRO_END) {
        goToMacroEnd(program);
    }
}

error addToken( token_t **tokens, int *capacity, int *idx, token_t tok )
{
    if (*idx >= *capacity) {
        *capacity *= 2;
        if ( (*tokens = realloc( *tokens, sizeof(token_t) * (*capacity))) == NULL )
            return ENOMEM;
    }

    (*tokens)[*idx] = tok;
    (*idx)++;

    return EXIT_SUCCESS;
}

error tokenizeMacro( program_t *program, MACRO_T *m )
{
    int capacity = 10;
    int rv;

    token_t *tokens;
    if ( (tokens = malloc(sizeof(token_t) * capacity)) == NULL )
         return ENOMEM;
        
    token_t *params;
    if ( (params = malloc(sizeof(token_t) * capacity)) == NULL )
         return ENOMEM;

    token_t tok;
    int idx = 0;
    int idx2 = 0;
    bool get_params = true;
    char *name = NULL;

    while( ++program->token_idx < program->n_tokens ){ 

        tok = program->tokens[program->token_idx];

        if (tok.type == TOK_MACRO_START) {
            goToMacroEnd(program);
            continue;
        } else if (tok.type == TOK_MACRO_END) {
            break;
        }

        if (name == NULL) {
            name = malloc( strlen(tok.token) + 1 );
            strcpy( name, tok.token );
            
            continue;
        }

        if (get_params) {
            rv = addToken(&params, &capacity, &idx2, tok);
            //printf("NEW PARAM: %s\n", tok.token);
    
            if (tok.type == TOK_NEWLINE) {
                get_params = false;
            }
        } else {
            rv = addToken(&tokens, &capacity, &idx, tok);
        }
        if (rv)
          return rv;
    }

    if (idx == 0) {
        free(tokens);
        tokens = NULL;
        idx = 0;
        params = NULL;
        idx2 = 0;
    }

    m->name     = strdup(name);
    m->tokens   = tokens;
    m->n_tokens = idx;
    m->params   = params;
    m->n_params = idx2;

    return EXIT_SUCCESS;
}

void defineMode( program_t *program )
{
    MACRO_T *m = malloc( sizeof( MACRO_T ) ); 
    tokenizeMacro(program, m);

    if (m->name != NULL) {
        add_macro(program, m);
    }
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
            break;
        }
        program->token_idx++;
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

