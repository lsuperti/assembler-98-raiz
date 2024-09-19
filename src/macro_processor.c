
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
            fprintf(stdout,
            "\nMACRO : %s, NUM_TOKENS : %lu, NUM_PARAMS : %lu\n",  entry->name,
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

token_t *peek_token_idx( program_t *program, int *idx )
{
    assert( program != NULL );
    if ( program->token_idx < program->n_tokens ) 
    {
        return &program->tokens[ *idx ];
    }else 
    {
        return NULL;
    }
}

error tokenizeMacro( program_t *program, MACRO_T *m )
{
    int capacity = 10;
    int rv;

    ++program->token_idx;
    m->n_params = compute_nargs( program );
    m->name = strdup(program->tokens[ program->token_idx ].token);

    token_t *tokens;
    if ( (tokens = malloc(sizeof(token_t) * capacity)) == NULL )
         return ENOMEM;
        
    token_t *params;
    if ( (params = malloc(sizeof(token_t) * capacity)) == NULL )
         return ENOMEM;

    MACRO_T *local_macros;
    if ( (local_macros = malloc(sizeof(MACRO_T) * capacity)) == NULL )
         return ENOMEM;
    
    token_t tok;

    int idx = 0;
    int idx2 = 0;
    int idx3 = 0;

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

    if ( idx <= 0 )
    {
        free(tokens);
        free(params);
        free(local_macros);
        
        idx = 0;
        tokens = NULL;

        idx2 = 0;
        params = NULL;
        
        idx3 = 0;
        local_macros = NULL;
    }

    m->tokens   = tokens;
    m->n_tokens = idx;
    m->params   = params;
    m->n_params = idx2;
    m->local_macros = local_macros;
    m->n_local_macros = idx3;

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

token_t *createBlankToken ( int line, int column, int offset ) 
{
    token_t *bt;
    if ( ( bt = malloc ( sizeof ( token_t ) ) ) == NULL )
        return NULL;
    bt->token   = "BLANK";
    bt->type    = TOK_BLANK;
    bt->line    = line;
    bt->value   = -1;
    bt->column  = column;
    bt->offset  = offset;
    bt->defined = true;
    return bt;
}

// Call malloc or realloc before calling this otherwise
// this will blow up.
void shiftTokens_reverse_by_amnt( token_t *tokens, size_t last_tok,
        int tok_idx, int amnt ) 
{
   for ( int i=last_tok; i >= tok_idx; i-- ) 
   {
        token_t *blank_token =
            createBlankToken( tokens[i].line, tokens[i].column,
                    tokens[i].offset );

       // if ( blank_token == NULL )
       //     return;

        tokens[i + amnt] = tokens[i]; 
        tokens[i]        = *blank_token;
   }
}

// Expand mode for program->tokens
// Which will be different than the one used 
// when on defineMode.
error expandMode( 
        token_t **tokens, size_t *n_tokens, int tok_idx, MACRO_T *m )
{
    size_t new_size;
    new_size = ((*n_tokens) + m->n_tokens);

    if ( ( *tokens = realloc( *tokens, sizeof( token_t ) * new_size) )
         == NULL ) 
        return ENOMEM;

    shiftTokens_reverse_by_amnt( *tokens, (*n_tokens) - 1,
            tok_idx + m->n_params + 1, m->n_tokens );

    for ( int i=0; i < m->n_tokens; i++ ) 
    {
        if ( m->tokens != NULL )
        {
          (*tokens)[i + tok_idx] = m->tokens[i];
          fprintf(stdout,
                  "Replacing %s for %s\n", (*tokens)[i].token, m->tokens[i].token );
          fflush(stdout);
        }
    }

    *n_tokens = new_size;
    return EXIT_SUCCESS;
}


int compute_nargs( program_t *p ) 
{
    int idx = p->token_idx, n = 0;    
    ++idx;
    token_t tok = *peek_token_idx(p, &idx);

    while ( tok.type == TOK_IDENTIFIER  ||
            tok.type == TOK_LITERAL     ||
            tok.type == TOK_LITERAL_HEX ||
            tok.type == TOK_ADDRESSING   ) 
    {

        if ( tok.type == TOK_ADDRESSING ) 
        {
            ++idx;
            token_t t = *peek_token_idx(p, &idx);
            if ( t.type == TOK_IDENTIFIER  ||
                 t.type == TOK_LITERAL     ||
                 t.type == TOK_LITERAL_HEX ||
                 t.type == TOK_ADDRESSING   ) 
            {}else
            {
               return _EINVAL;
            }
        }
        ++idx;
        n++;    
        tok = *peek_token_idx(p, &idx);
    }

    return n;
}

void process_macros( program_t *program )
{
    int state = program->token_idx;
    program->token_idx = 0;
    token_t tok;
    MACRO_T *m;
    size_t  rv;
    while ( program->token_idx < program->n_tokens )
    {
        tok = program->tokens[program->token_idx];
        switch ( tok.type )
        {
            case TOK_MACRO_START:
                defineMode(program);
            break;
            case TOK_IDENTIFIER:
                m = find_macro( program, tok.token );
                int n = compute_nargs(program);
                if ( m != NULL ) 
                {
                    if ( n == m->n_params )
                    {
                        rv = expandMode(&(program->tokens), &(program->n_tokens),
                                    program->token_idx, m);
                        if (rv)
                           return;
                    }
                    else 
                        ;
                }
            break;
            default:
            break;
        }
        program->token_idx++;
    }
    program->token_idx = state;
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

