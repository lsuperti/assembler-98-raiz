
#include "macro_processor.h"
#include <errno.h>
#include "montador.h"

void resetIdentifiers_Macros( program_t *p ) 
{
    if ( p != NULL )
    {
        if ( p->table != NULL )
        {
            HASH_CLEAR(hh, p->table->tokens ); 
            p->table->num_s  = 0;
            p->table->tokens = NULL;
            p->table         = NULL;
        }
        HASH_CLEAR(hh, p->macros );
        HASH_CLEAR(hh, p->globals);
        HASH_CLEAR(hh, p->externs);
        p->n_globals     = 0;
        p->n_externs     = 0;
        p->n_macros      = 0;
        p->macros        = NULL;
    }
}

error_rv printMacros( program_t *p, FILE *f ) 
{
    MACRO_T *entry, *tmp; 

    program_t *dirty_hack;
    if ( ( dirty_hack = malloc(sizeof(program_t)) ) == NULL )
        return ENOMEM;

    HASH_ITER(hh, p->macros, entry, tmp)
    {
        if (entry)
        {
            fprintf(f,
            "\nMACRO : %s, NUM_TOKENS : %lu, NUM_PARAMS : %lu, NUM_LOCAL_MACROS : %lu\n",  entry->name,
            entry->n_tokens, entry->n_params, entry->n_local_macros   );
            for ( int i=0; i < entry->n_params; i++ )
            {
            }
            fflush(f);
            dirty_hack->tokens   = entry->tokens;
            dirty_hack->n_tokens = entry->n_tokens;
            printTokens( dirty_hack );
        }
    }

    free(dirty_hack);
    return EXIT_SUCCESS;
}

void tokens_out( program_t *p, FILE *f ) 
{
    if ( p->tokens == NULL )
        return;

    for ( int i=0; i < p->n_tokens; i++ )
    {
        token_t t = p->tokens[i];
        if ( t.token != NULL 
             && t.type != TOK_NEWLINE )
        {
            fprintf( f, "%s ", p->tokens[i].token );
            fflush(f);
            if ( ( i + 1 ) % 2 == 0 ) 
                fprintf(f, "\n");
        }
    }
}

error_rv addToken( token_t **tokens, int *capacity, int *idx, token_t tok )
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

error_rv addMacro( MACRO_T **local_macros, int *capacity, int *idx, MACRO_T *m )
{
    if (*idx >= *capacity) {
        *capacity *= 2;
        if ( (*local_macros = realloc( *local_macros, sizeof(MACRO_T) * (*capacity))) == NULL )
            return ENOMEM;
    }

    (*local_macros)[*idx] = *m;
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


// Instead of using blank tokens whenever there is a token
// replacement we could just shift down the tokens 
// i didn't do it because it seemed to be annoying to implement.
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

error_rv tokenizeMacro( program_t *program, MACRO_T *m )
{
    int capacity = 10;
    int rv;

    if ( program->token_idx < program->n_tokens ) 
    {
        token_t t = program->tokens[program->token_idx];
        program->tokens[program->token_idx] 
            = *createBlankToken( t.line, t.column, t.offset );
    }

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

    bool get_params   = true;
    char *name = NULL;

    while( ++program->token_idx < program->n_tokens ){ 

        tok = program->tokens[program->token_idx];

        program->tokens[program->token_idx] =
            *createBlankToken( tok.line, tok.column, tok.offset );

        if (name == NULL) {
            if ( ( name = malloc( strlen(tok.token) + 1 ) ) == NULL )
                return ENOMEM;
            strcpy(name, tok.token);
            continue;
        }

        if (tok.type == TOK_MACRO_START) {
            MACRO_T *m2 = malloc( sizeof( MACRO_T ) ); 
            tokenizeMacro(program, m2);
            addMacro(&local_macros, &capacity, &idx3, m2);
            continue;
        } else if (tok.type == TOK_MACRO_END) 
            break;

        if (get_params) {
            if ( tok.type == TOK_PARAM_DELIMITER ||
                 tok.type != TOK_IDENTIFIER ) {
                get_params = false;
                rv = addToken(&tokens, &capacity, &idx, tok);
            }else {
                rv = addToken(&params, &capacity, &idx2, tok);
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
        tokens = NULL;
        idx = 0;
        params = NULL;
        idx2 = 0;
        local_macros = NULL;
        idx3 = 0;
    }

    m->name     = name;
    m->tokens   = tokens;
    m->n_tokens = idx;
    m->params   = params;
    m->n_params = idx2;
    m->called   = 0;
    m->local_macros = local_macros;
    m->n_local_macros = idx3;

    return EXIT_SUCCESS;
}

void defineMode( program_t *program )
{
    MACRO_T *m = malloc( sizeof( MACRO_T ) ); 
    tokenizeMacro(program, m);

    if (m->name != NULL) {
        MACRO_T *o_m = find_macro( program, m->name );
        if ( o_m == NULL ) 
            add_macro(program, m);
        else 
        {
            replace_macro(program, m, o_m);
            free(o_m);
        }
    }
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

        tokens[i + amnt] = tokens[i]; 
        tokens[i]        = *blank_token;
   }
}

int calculateDigits(int n) {
    if (n < 0) return calculateDigits((n == INT_MIN) ? INT_MAX: -n);
    if (n < 10) return 1;
    return 1 + calculateDigits(n / 10);
}

void trimToken(token_t *t) {
    int len = strlen(t->token);
    
    if (len <= 3) {
        t->token[0] = '\0';
        return;
    }
    memmove(t->token, t->token + 2, len - 3);
    t->token[len - 3] = '\0';
}

// Expand mode for program->tokens
// Which will be different than the one used 
// when on defineMode.
error_rv expandMode( 
  token_t **tokens, size_t *n_tokens, int tok_idx, MACRO_T *m, token_t *p_values
  , program_t *program )
{
    size_t new_size;
    new_size = ((*n_tokens) + m->n_tokens);

    if ( ( *tokens = realloc( *tokens, sizeof( token_t ) * new_size) )
         == NULL ) 
        return ENOMEM;

    // Shifting of every token so that the macro
    // tokens can be placed where the macro was called
    // without affecting the tokens that were already 
    // present.
    shiftTokens_reverse_by_amnt( *tokens, (*n_tokens) - 1,
            tok_idx + m->n_params + 1, m->n_tokens );

    // Macro token replacement on start of call.
    for ( int i=0; i < m->n_tokens; i++ ) 
    {
        if ( m->tokens != NULL )
        {
          (*tokens)[i + tok_idx] = m->tokens[i];
        }
    }

    // Formal parameter replacement for actual values.
    bool ad_tk = false;
    int k = 0;
    for ( int j=0; j < m->n_params; j++ )
    {

        token_t c_param_formal = m->params[j];
        token_t c_param_value  = p_values[k];

        for ( int i=0; i < m->n_tokens; i++ )
        {
            if ( m->tokens != NULL )
            {
               
                if ( strncmp((*tokens)[i + tok_idx].token, c_param_formal.token,
                            strlen(c_param_formal.token) )
                        == 0 ) 
                {
                    if ( c_param_value.type != TOK_ADDRESSING )
                        (*tokens)[i + tok_idx] = c_param_value;
                    else 
                    {
                       // Não consegui fazer funcionar se
                       // o argumento pra macro é algo como &20 ou &ident
                       // depois eu arrumo.

                      //  new_size++;
                      //  if ( ( *tokens =
                      //    realloc( *tokens, sizeof( token_t ) *  new_size  ) )
                      //    == NULL ) 
                      //          return ENOMEM;

                      //  ad_tk = true;

                      //  shiftTokens_reverse_by_amnt( *tokens, new_size - 1,
                      //          i + tok_idx, 2 ); 
                      //  (*tokens)[i + tok_idx]     = c_param_value;
                      //  (*tokens)[i + tok_idx + 1] = p_values[k + 1];
                    }

                }
                
            }
        }
        if ( ad_tk ) 
            k+=2;
        else 
            k++;
    }

    // Replacement of local labels and identifiers for
    // a generalized call by the convention : 
    // { macro_name }_{ hash }_{ called }_{ (label || ident)_name }
    for ( int i=0; i < m->n_tokens; i++ )
    {
        token_t t = (* tokens)[ i + tok_idx ];
        unsigned long h = hash( (unsigned char *) m->name );
        char *orf;
        char *h_prefix = malloc( 21 );
        sprintf( h_prefix, "%lu", h);
        size_t len = strlen(t.token);

        if ( t.type == TOK_LOCAL_LABEL )
        {
            if ( len >= 2 )
            {
                orf = malloc( strlen(t.token) + 1 );
                for ( int i=2; i < len; i++ )
                    orf[i - 2] = t.token[i];
                orf[ len - 2 ] = '\0';
            }else {
                orf = malloc(1); 
                orf[0] = '\0';  
            }

            char   *called_str = malloc( calculateDigits( m->called ) + 1 );
            sprintf( called_str, "%lu", m->called );

            char *lb_name = malloc( strlen(m->name) + 
                            strlen(h_prefix) + strlen(called_str)
                            + strlen(orf) + 8 );

            strcpy( lb_name, m->name );
            strcat( lb_name, "_" );
            // Fazer a hash depois.
            strcat( lb_name, h_prefix );
            strcat( lb_name, "_" );
            strcat( lb_name, called_str);
            strcat( lb_name, "_" );
            strcat( lb_name, orf );
            (* tokens)[ i + tok_idx ].type  = TOK_LABEL;
            (* tokens)[ i + tok_idx ].token = lb_name;

        }else if ( t.type == TOK_LOCAL_IDENTIFIER )
        {
            orf = malloc( strlen(t.token)  +  1);
            int len = strlen(t.token);
            for ( int i=2; i < len; i++ )
                orf[i - 2] = t.token[i];
            orf[ len - 2 ] = '\0';

            char   *called_str = malloc( calculateDigits( m->called ) + 1 );
            sprintf( called_str, "%lu", m->called );

            char *lb_name = malloc( strlen(m->name) + 
                            strlen(h_prefix) + strlen(called_str)
                            + strlen(orf) + 8 );

            strcpy( lb_name, m->name );
            strcat( lb_name, "_" );
            // Fazer a hash depois.
            strcat( lb_name, h_prefix );
            strcat( lb_name, "_" );
            strcat( lb_name, called_str);
            strcat( lb_name, "_" );
            strcat( lb_name, orf );


            (* tokens)[ i + tok_idx ].type  = TOK_IDENTIFIER;
            (* tokens)[ i + tok_idx ].token = lb_name;

        }
    }

    *n_tokens = new_size;
    bool local = false;

    // Define all local macros for this expanded macro
    // put them in the global program->macros
    for ( int i=0; i < m->n_local_macros; i++ ) 
    {
        MACRO_T *c_l = &m->local_macros[i];
        for ( int j=0; j < m->n_params; j++ )
        {
            for ( int k=0; k < c_l->n_params; k++ )
            {
                
                    if ( strncmp(c_l->params[k].formal, m->params[j].formal, 
                                 strlen(c_l->params[k].formal) ) == 0 )
                    {
                         local = true;
                         break;
                    }
            }

            if ( !local )
            {
                // Formals
                token_t param = m->params[j];
                for ( int b=0; b < c_l->n_tokens; b++ )
                {
                    token_t  tok = c_l->tokens[b]; 
                    if ( tok.formal != NULL )
                    {
                       if ( strncmp( param.formal, c_l->tokens[b].token, 
                            strlen(c_l->tokens[b].token) ) == 0 ) 
                       {
                            p_values[j].formal = strdup(c_l->tokens[b].formal);
                            c_l->tokens[b] = p_values[j]; 
                       }
                    }
                }
            }
            local = false;
        }
        MACRO_T *mt = find_macro( program, c_l->name );
        if ( mt == NULL )
            add_macro(program, &m->local_macros[i]);
        else 
        {
            replace_macro(program, &m->local_macros[i], mt);
        }
    }

    return EXIT_SUCCESS;
}

int compute_nargs( program_t *p ) 
{
    int cap = 10;
    int idx = p->token_idx, n = 0;    
    ++idx;
    token_t tok = *peek_token_idx(p, &idx);
    int params = 0;

    if ( p->cur_macro_params != NULL )
        free(p->cur_macro_params);

    if ( (p->cur_macro_params = malloc( sizeof(token_t) * cap )) == NULL )
         return _ENOMEM;

    while ( tok.type == TOK_IDENTIFIER  ||
            tok.type == TOK_LITERAL     ||
            tok.type == TOK_LITERAL_HEX ||
            tok.type == TOK_ADDRESSING   ) 
    {

        if ( tok.type == TOK_ADDRESSING ) 
        {
            addToken( &(p->cur_macro_params), &cap, &params, tok);
            ++idx;
            token_t t = *peek_token_idx(p, &idx);
            addToken( &(p->cur_macro_params), &cap, &params, tok);
            if ( t.type == TOK_IDENTIFIER  ||
                 t.type == TOK_LITERAL     ||
                 t.type == TOK_LITERAL_HEX ||
                 t.type == TOK_ADDRESSING   ) 
            {}else
            {
               return _EINVAL;
            }
        }else if ( tok.type == TOK_IDENTIFIER ) 
        {
            MACRO_T *m = find_macro( p, tok.token );
            if ( m != NULL )
                 return n;
        }

        if ( tok.type != TOK_ADDRESSING )
            addToken( &(p->cur_macro_params), &cap, &params, tok);

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
    bool has_expanded = false;

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
                        has_expanded = true;
                        ++m->called;
                        rv = expandMode(&(program->tokens), &(program->n_tokens),
                                    program->token_idx, m,
                                    program->cur_macro_params, program);
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

    FILE *f;
    if ( (f = fopen(current_macro_out, "w")) == NULL )
        return;

    if ( has_expanded )
    {
        GtkTextView *c =
            GTK_TEXT_VIEW(gtk_builder_get_object( p_builder, "consoleErros" ));
        append_text_to_text_view(c, "Macros expanded...\n");
    }

    tokens_out(program, f);
    fclose(f);
}

void add_macro( program_t *program, MACRO_T *macro ) 
{
    HASH_ADD_STR( program->macros, name, macro );
}

// m1 -> m2 ;
void replace_macro( program_t *program, MACRO_T *m1, MACRO_T *m2 )
{
    HASH_REPLACE_STR( program->macros, name, m1, m2 );
}

MACRO_T *find_macro( program_t *program, char *name )
{
    MACRO_T *m;
    HASH_FIND_STR( program->macros, name, m);
    return m;
}

