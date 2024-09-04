
#include "parser.h"

// recursive descent parsing
// gramatica não-terminal 

// LOAD 20
// LOAD &20
// LOAD #20
int parseLoad( program_t *program, token_t *c_tok ) 
{
    token_t *peeked_1 = getNextToken( program ), *peeked_2;
    bool defined;
    switch ( peeked_1->type ) 
    {
        case TOK_ADDRESSING:
            peeked_2 = getNextToken( program );
            if ( strncmp( peeked_1->token, "&", 2 ) == 0 )
            {
                insert( program->sections->dot_text, LOAD_DIRECT  );
            }else 
            {
                insert( program->sections->dot_text, LOAD_INDIRECT );
            }
            insert( program->sections->dot_text, peeked_2->value );
            break;
        case TOK_LITERAL:
        case TOK_LITERAL_HEX:
            insert( program->sections->dot_text, LOAD_IMMEDIATE  );
            insert( program->sections->dot_text, peeked_1->value );
            break;
        case TOK_IDENTIFIER:
            defined = false;
            for ( int i=0; i < program->table->num_s; i++ )
            {
                if ( strcmp( program->table->tokens[i].token, peeked_1->token ) == 0 )
                {
                    defined = true;    
                    insert( program->sections->dot_text, LOAD_DIRECT );
                    insert( program->sections->dot_text,
                        program->table->tokens[i].value );
                }
            }
        
            if ( !defined ) 
            {
                current_parser_error = 
                    realloc ( current_parser_error, 25 + strlen(peeked_1->token) );
                sprintf( current_parser_error, 
                        "Undefined identifier : %s", peeked_1->token );
                return -1;
            }

            break;
        default:
            // Expected token type ( TOK_ADDRESSING || TOK_LITERAL
            // || TOK_LITERAL_HEX ) 
            // got : peeked_1->type
            return -1;
    }

    return EXIT_SUCCESS;
}

// STORE #29
// STORE &29
// STORE Var
int parseStore( program_t *program, token_t *c_tok ) 
{
    token_t *peeked_1 = getNextToken( program ), *peeked_2;
    bool defined;
    switch ( peeked_1->type ) 
    {
        case TOK_ADDRESSING:
            peeked_2 = getNextToken( program );
            if ( strncmp( peeked_1->token, "&", 2 ) == 0 )
            {
                insert( program->sections->dot_text, STORE_DIRECT  );
            }else 
            {
                insert( program->sections->dot_text, STORE_INDIRECT );
            }
            insert( program->sections->dot_text, peeked_2->value );
            break;
        case TOK_IDENTIFIER:
            defined = false;
            for ( int i=0; i < program->table->num_s; i++ )
            {
                if ( strcmp( program->table->tokens[i].token, peeked_1->token ) == 0 )
                {
                    defined = true;    
                    insert( program->sections->dot_text, STORE_DIRECT );
                    insert( program->sections->dot_text,
                        program->table->tokens[i].value );
                }
            }
        
            if ( !defined ) 
            {
                // Undefined identifier : peeked_1->token
                return -1;
            }
              
            break;
        default:
            // Expected token type ( TOK_ADDRESSING )
            // got : peeked_1->type
            return -1;
            break;
    }

    return EXIT_SUCCESS;
}

