
#include "parser.h"

// LOAD 20
// LOAD &20
// LOAD #20
int parseLoad( program_t *program, token_t *c_tok ) 
{
    token_t *peeked_1 = getNextToken( program ), *peeked_2;
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
int parseStore( program_t *program, token_t *c_tok ) 
{
    token_t *peeked_1 = getNextToken( program ), *peeked_2;
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
        default:
            // Expected token type ( TOK_ADDRESSING )
            // got : peeked_1->type
            return -1;
            break;
    }

    return EXIT_SUCCESS;
}

