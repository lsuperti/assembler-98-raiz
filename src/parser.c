
#include "parser.h"

// recursive descent parsing
// gramatica não-terminal 

int parseSection( program_t *program, token_t *c_tok ) 
{
    token_t *peeked_1 = getNextToken( program ), *peeked_2;
    bool defined;
    switch ( peeked_1->type ) 
    {
        case TOK_ADDRESSING:
            peeked_2 = getNextToken( program );
            if ( strncmp( peeked_1->token, "&", 2 ) == 0 )
            {
                insert( program->sections->dot_text, PUT_DIRECT );
            }else 
            {
                insert( program->sections->dot_text, PUT_INDIRECT );
            }
            insert( program->sections->dot_text, peeked_2->value );
            break;
        case TOK_LITERAL:
        case TOK_LITERAL_HEX:
            insert( program->sections->dot_text, PUT_IMMEDIATE );
            insert( program->sections->dot_text, peeked_1->value );
            break;
        case TOK_IDENTIFIER:
            defined = false;
            for ( int i=0; i < program->table->num_s; i++ )
            {
                if ( strcmp( program->table->tokens[i].token, peeked_1->token ) == 0 )
                {
                    defined = true;    
                    insert( program->sections->dot_text, PUT_DIRECT );
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

int parsePUT( program_t *program, token_t *c_tok ) 
{
    token_t *peeked_1 = getNextToken( program ), *peeked_2;
    bool defined;
    switch ( peeked_1->type ) 
    {
        case TOK_ADDRESSING:
            peeked_2 = getNextToken( program );
            if ( strncmp( peeked_1->token, "&", 2 ) == 0 )
            {
                insert( program->sections->dot_text, PUT_DIRECT );
            }else 
            {
                insert( program->sections->dot_text, PUT_INDIRECT );
            }
            insert( program->sections->dot_text, peeked_2->value );
            break;
        case TOK_LITERAL:
        case TOK_LITERAL_HEX:
            insert( program->sections->dot_text, PUT_IMMEDIATE );
            insert( program->sections->dot_text, peeked_1->value );
            break;
        case TOK_IDENTIFIER:
            defined = false;
            for ( int i=0; i < program->table->num_s; i++ )
            {
                if ( strcmp( program->table->tokens[i].token, peeked_1->token ) == 0 )
                {
                    defined = true;    
                    insert( program->sections->dot_text, PUT_DIRECT );
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

int parseRead( program_t *program, token_t *c_tok ) 
{
    token_t *peeked_1 = getNextToken( program ), *peeked_2;
    bool defined;
    switch ( peeked_1->type ) 
    {
        case TOK_ADDRESSING:
            peeked_2 = getNextToken( program );
            if ( strncmp( peeked_1->token, "&", 2 ) == 0 )
            {
                insert( program->sections->dot_text, READ_DIRECT );
            }else 
            {
                insert( program->sections->dot_text, READ_INDIRECT );
            }
            insert( program->sections->dot_text, peeked_2->value );
            break;
        case TOK_LITERAL:
        case TOK_LITERAL_HEX:
            insert( program->sections->dot_text, READ_DIRECT );
            insert( program->sections->dot_text, peeked_1->value );
            break;
        case TOK_IDENTIFIER:
            defined = false;
            for ( int i=0; i < program->table->num_s; i++ )
            {
                if ( strcmp( program->table->tokens[i].token, peeked_1->token ) == 0 )
                {
                    defined = true;    
                    insert( program->sections->dot_text, READ_DIRECT );
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

int parseBRPOS( program_t *program, token_t *c_tok ) 
{
    token_t *peeked_1 = getNextToken( program ), *peeked_2;
    bool defined;
    switch ( peeked_1->type ) 
    {
        case TOK_ADDRESSING:
            peeked_2 = getNextToken( program );
            if ( strncmp( peeked_1->token, "&", 2 ) == 0 )
            {
                insert( program->sections->dot_text, BRPOS_DIRECT   );
            }else 
            {
                insert( program->sections->dot_text, BRPOS_INDIRECT );
            }
            insert( program->sections->dot_text, peeked_2->value );
            break;
        case TOK_LITERAL:
        case TOK_LITERAL_HEX:
            insert( program->sections->dot_text, BRPOS_IMMEDIATE    );
            insert( program->sections->dot_text, peeked_1->value );
            break;
        case TOK_IDENTIFIER:
            defined = false;
            for ( int i=0; i < program->table->num_s; i++ )
            {
                if ( strcmp( program->table->tokens[i].token, peeked_1->token ) == 0 )
                {
                    defined = true;    
                    insert( program->sections->dot_text, BRPOS_IMMEDIATE );
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

int parseBRZERO( program_t *program, token_t *c_tok ) 
{
    token_t *peeked_1 = getNextToken( program ), *peeked_2;
    bool defined;
    switch ( peeked_1->type ) 
    {
        case TOK_ADDRESSING:
            peeked_2 = getNextToken( program );
            if ( strncmp( peeked_1->token, "&", 2 ) == 0 )
            {
                insert( program->sections->dot_text, BRZERO_DIRECT   );
            }else 
            {
                insert( program->sections->dot_text, BRZERO_INDIRECT );
            }
            insert( program->sections->dot_text, peeked_2->value );
            break;
        case TOK_LITERAL:
        case TOK_LITERAL_HEX:
            insert( program->sections->dot_text, BRZERO_IMMEDIATE    );
            insert( program->sections->dot_text, peeked_1->value );
            break;
        case TOK_IDENTIFIER:
            defined = false;
            for ( int i=0; i < program->table->num_s; i++ )
            {
                if ( strcmp( program->table->tokens[i].token, peeked_1->token ) == 0 )
                {
                    defined = true;    
                    insert( program->sections->dot_text, BRZERO_IMMEDIATE );
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

int parseBRNEG( program_t *program, token_t *c_tok ) 
{
    token_t *peeked_1 = getNextToken( program ), *peeked_2;
    bool defined;
    switch ( peeked_1->type ) 
    {
        case TOK_ADDRESSING:
            peeked_2 = getNextToken( program );
            if ( strncmp( peeked_1->token, "&", 2 ) == 0 )
            {
                insert( program->sections->dot_text, BRNEG_DIRECT   );
            }else 
            {
                insert( program->sections->dot_text, BRNEG_INDIRECT );
            }
            insert( program->sections->dot_text, peeked_2->value );
            break;
        case TOK_LITERAL:
        case TOK_LITERAL_HEX:
            insert( program->sections->dot_text, BRNEG_IMMEDIATE    );
            insert( program->sections->dot_text, peeked_1->value );
            break;
        case TOK_IDENTIFIER:
            defined = false;
            for ( int i=0; i < program->table->num_s; i++ )
            {
                if ( strcmp( program->table->tokens[i].token, peeked_1->token ) == 0 )
                {
                    defined = true;    
                    insert( program->sections->dot_text, BRNEG_IMMEDIATE );
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

int parseBR( program_t *program, token_t *c_tok ) 
{
    token_t *peeked_1 = getNextToken( program ), *peeked_2;
    bool defined;
    switch ( peeked_1->type ) 
    {
        case TOK_ADDRESSING:
            peeked_2 = getNextToken( program );
            if ( strncmp( peeked_1->token, "&", 2 ) == 0 )
            {
                insert( program->sections->dot_text, BR_DIRECT   );
            }else 
            {
                insert( program->sections->dot_text, BR_INDIRECT );
            }
            insert( program->sections->dot_text, peeked_2->value );
            break;
        case TOK_LITERAL:
        case TOK_LITERAL_HEX:
            insert( program->sections->dot_text, BR_IMMEDIATE    );
            insert( program->sections->dot_text, peeked_1->value );
            break;
        case TOK_IDENTIFIER:
            defined = false;
            for ( int i=0; i < program->table->num_s; i++ )
            {
                if ( strcmp( program->table->tokens[i].token, peeked_1->token ) == 0 )
                {
                    defined = true;    
                    insert( program->sections->dot_text, BR_IMMEDIATE );
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
// .word 20
// .word 0x30
int parseWord( program_t *program, token_t *c_tok ) 
{
    token_t *peeked_1 = getNextToken( program );
    switch ( peeked_1->type ) 
    {
        case TOK_LITERAL:
        case TOK_LITERAL_HEX:
            insert( program->sections->dot_data, peeked_1->value );
            break;
        default:
            current_parser_error = 
                realloc ( current_parser_error, 50 + strlen(peeked_1->token) );
            sprintf( current_parser_error, 
                    "Expected ( TOK_LITERAL || TOK_LITERAL_HEX )"
                    " Found : %s", peeked_1->token );
            return -1;
    }

    return EXIT_SUCCESS;
}

int parseMULT( program_t *program, token_t *c_tok ) 
{
    token_t *peeked_1 = getNextToken( program ), *peeked_2;
    bool defined;
    switch ( peeked_1->type ) 
    {
        case TOK_ADDRESSING:
            peeked_2 = getNextToken( program );
            if ( strncmp( peeked_1->token, "&", 2 ) == 0 )
            {
                insert( program->sections->dot_text, MULT_DIRECT );
            }else 
            {
                insert( program->sections->dot_text, MULT_INDIRECT );
            }
            insert( program->sections->dot_text, peeked_2->value );
            break;
        case TOK_LITERAL:
        case TOK_LITERAL_HEX:
            insert( program->sections->dot_text, MULT_IMMEDIATE );
            insert( program->sections->dot_text, peeked_1->value );
            break;
        case TOK_IDENTIFIER:
            defined = false;
            for ( int i=0; i < program->table->num_s; i++ )
            {
                if ( strcmp( program->table->tokens[i].token, peeked_1->token ) == 0 )
                {
                    defined = true;    
                    insert( program->sections->dot_text, MULT_DIRECT );
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

int parseSUB( program_t *program, token_t *c_tok ) 
{
    token_t *peeked_1 = getNextToken( program ), *peeked_2;
    bool defined;
    switch ( peeked_1->type ) 
    {
        case TOK_ADDRESSING:
            peeked_2 = getNextToken( program );
            if ( strncmp( peeked_1->token, "&", 2 ) == 0 )
            {
                insert( program->sections->dot_text, SUB_DIRECT  );
            }else 
            {
                insert( program->sections->dot_text, SUB_INDIRECT );
            }
            insert( program->sections->dot_text, peeked_2->value );
            break;
        case TOK_LITERAL:
        case TOK_LITERAL_HEX:
            insert( program->sections->dot_text, SUB_IMMEDIATE );
            insert( program->sections->dot_text, peeked_1->value );
            break;
        case TOK_IDENTIFIER:
            defined = false;
            for ( int i=0; i < program->table->num_s; i++ )
            {
                if ( strcmp( program->table->tokens[i].token, peeked_1->token ) == 0 )
                {
                    defined = true;    
                    insert( program->sections->dot_text, SUB_DIRECT );
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

int parseDIVIDE( program_t *program, token_t *c_tok ) 
{
    token_t *peeked_1 = getNextToken( program ), *peeked_2;
    bool defined;
    switch ( peeked_1->type ) 
    {
        case TOK_ADDRESSING:
            peeked_2 = getNextToken( program );
            if ( strncmp( peeked_1->token, "&", 2 ) == 0 )
            {
                insert( program->sections->dot_text, DIV_DIRECT );
            }else 
            {
                insert( program->sections->dot_text, DIV_INDIRECT );
            }
            insert( program->sections->dot_text, peeked_2->value );
            break;
        case TOK_LITERAL:
        case TOK_LITERAL_HEX:
            insert( program->sections->dot_text, DIV_IMMEDIATE );
            insert( program->sections->dot_text, peeked_1->value );
            break;
        case TOK_IDENTIFIER:
            defined = false;
            for ( int i=0; i < program->table->num_s; i++ )
            {
                if ( strcmp( program->table->tokens[i].token, peeked_1->token ) == 0 )
                {
                    defined = true;    
                    insert( program->sections->dot_text, DIV_DIRECT );
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

int parseADD( program_t *program, token_t *c_tok ) 
{
    token_t *peeked_1 = getNextToken( program ), *peeked_2;
    bool defined;
    switch ( peeked_1->type ) 
    {
        case TOK_ADDRESSING:
            peeked_2 = getNextToken( program );
            if ( strncmp( peeked_1->token, "&", 2 ) == 0 )
            {
                insert( program->sections->dot_text, ADD_DIRECT );
            }else 
            {
                insert( program->sections->dot_text, ADD_INDIRECT );
            }
            insert( program->sections->dot_text, peeked_2->value );
            break;
        case TOK_LITERAL:
        case TOK_LITERAL_HEX:
            insert( program->sections->dot_text, ADD_IMMEDIATE );
            insert( program->sections->dot_text, peeked_1->value );
            break;
        case TOK_IDENTIFIER:
            defined = false;
            for ( int i=0; i < program->table->num_s; i++ )
            {
                if ( strcmp( program->table->tokens[i].token, peeked_1->token ) == 0 )
                {
                    defined = true;    
                    insert( program->sections->dot_text, ADD_DIRECT );
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
                    realloc ( current_parser_error, 100 + strlen(peeked_1->token) );
                snprintf( current_parser_error, 100, 
                        "Undefined identifier { %s } at line : { %d }", peeked_1->token,
                        peeked_1->line );
                gtk_text_buffer_set_text(cpe, current_parser_error, -1);
                return -1;
            }

            break;
        default:
            current_parser_error = ( char * ) malloc( 150 ); 
            snprintf( current_parser_error,
               150, 
    "Expected token ( TOK_IDENTIFIER || TOK_ADDRESSING || TOK_LITERAL ||"
    " TOK_LITERAL_HEX ) "
    "Found : %s at line : %d",
               peeked_1->token, peeked_1->line ) ;
            gtk_text_buffer_set_text(cpe, current_parser_error, -1);
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
            if ( peeked_2->type == TOK_IDENTIFIER )
            {
            defined = false;
            for ( int i=0; i < program->table->num_s; i++ )
            {
                if ( strcmp( program->table->tokens[i].token, peeked_2->token ) == 0 )
                {
                    defined = true;
                    insert( program->sections->dot_text,
                        program->table->tokens[i].value );
                }
            }

            if ( !defined )
                goto not_defined;

            }else 
            {
                insert( program->sections->dot_text, peeked_2->value );
            }
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
        not_defined:
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

