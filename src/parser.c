
#include "parser.h"

// recursive descent parsing
// gramatica não-terminal 

int parseGLOBAL( program_t *program, token_t *c_tok ) 
{
    token_t *peeked_1 = getNextToken( program );
    bool defined;
    token_t *t;

    switch ( peeked_1->type ) 
    {
        case TOK_IDENTIFIER:
            HASH_FIND_STR( program->table->tokens, peeked_1->token, t);
            if ( t == NULL )
            {
                current_parser_error = 
                    realloc ( current_parser_error, 100 + strlen(peeked_1->token) );
                snprintf( current_parser_error, 100,
                        "{ %s } Defined as global but not found on module.",
                        peeked_1->token );
                return -1;
            }
            else 
            {
                token_t *t2;
                HASH_FIND_STR( program->globals, peeked_1->token, t2 );
                
                peeked_1->value  = t->value;
                peeked_1->data_l = t->data_l;
                if ( t2 == NULL )
                    HASH_ADD_STR( program->globals, token, peeked_1 );
                else
                {
                    HASH_REPLACE_STR( program->globals, token, peeked_1,
                            t2 );
                }

            }
            break;
        default:
            current_parser_error = 
                realloc ( current_parser_error, 100 + strlen(peeked_1->token) );
            snprintf( current_parser_error, 100,
                    "Expected token TOK_IDENTIFIER found : %s",
                    peeked_1->token );
            return -1;
    }

    return EXIT_SUCCESS;
}
int parseCALL( program_t *program, token_t* c_tok)
{
    token_t *peeked_1 = getNextToken( program ), *peeked_2;
    bool defined;
    token_t *t;

    switch ( peeked_1->type ) 
    {
        case TOK_ADDRESSING:
            peeked_2 = getNextToken( program );
            if ( strncmp( peeked_1->token, "&", 2 ) == 0 )
            {
                insert( program->sections->dot_text, CALL_DIRECT );
            }else 
            {
                insert( program->sections->dot_text, CALL_INDIRECT );
            }

            if ( peeked_2->type == TOK_IDENTIFIER )
            {
                HASH_FIND_STR( program->table->tokens, peeked_2->token, t );
                if ( t == NULL )
                    goto not_defined;
                else 
                    insert( program->sections->dot_text, t->value );
            }else if ( peeked_2->type == TOK_LITERAL ||
                       peeked_2->type == TOK_LITERAL_HEX ) {
                    insert( program->sections->dot_text, peeked_2->value );
            }else 
            {
                current_parser_error = 
                    realloc ( current_parser_error, 100 + strlen(peeked_1->token) );
                sprintf( current_parser_error, 
                    "Expected token type ( TOK_LITERAL ||"
                    " TOK_IDENTIFIER || TOK_LITERAL_HEX )"
                    " found : %s", peeked_1->token );
                return -1;
            }
            break;
        case TOK_LITERAL:
        case TOK_LITERAL_HEX:
            insert( program->sections->dot_text, CALL_IMMEDIATE );
            insert( program->sections->dot_text, peeked_1->value );
            break;
        case TOK_IDENTIFIER: {
            HASH_FIND_STR( program->table->tokens, peeked_1->token, t );
            if ( t == NULL ) 
            {
        not_defined:
                current_parser_error = 
                    realloc ( current_parser_error, 25 + strlen(peeked_1->token) );
                sprintf( current_parser_error, 
                        "Undefined identifier : %s", peeked_1->token );
                return -1;
            }else {
                    insert( program->sections->dot_text, CALL_IMMEDIATE );
                    insert( program->sections->dot_text, t->value    );
            }

        }
        break;
        default:
            current_parser_error = 
                realloc ( current_parser_error, 100 + strlen(peeked_1->token) );
            snprintf( current_parser_error, 100,
                    "Expected token ( TOK_ADDRESSING ||"
                    " TOK_IDENTIFIER || TOK_LITERAL"
                    " || TOK_LITERAL_HEX ) found : %s", peeked_1->token );
            return -1;
    }

    return EXIT_SUCCESS;
}

int parseEXTERN( program_t *program, token_t *c_tok ) 
{
    token_t *peeked_1 = getNextToken( program );
    bool defined;
    token_t *t;

    switch ( peeked_1->type ) 
    {
        case TOK_IDENTIFIER:
            HASH_FIND_STR( program->table->tokens, peeked_1->token, t);
            if ( t == NULL )
            {
                // Se o identificador externo não tiver sido
                // utilizado já localmente ou por outra definição
                // de extern.
                peeked_1->value = 0x0;
                HASH_ADD_STR( program->table->tokens, token, peeked_1 );

                Vector xs     = find_all_identifier_pos( program, peeked_1 ); 
                peeked_1->pos = xs;

                token_t *e;
                HASH_FIND_STR( program->externs, peeked_1->token, e );
                if (e == NULL)
                    HASH_ADD_STR( program->externs, token, peeked_1 ); 
                else 
                    HASH_REPLACE_STR( program->externs, token, peeked_1,
                                      e );

            }
            else 
            {
                current_parser_error = 
                    realloc ( current_parser_error, 100 + strlen(peeked_1->token) );
                snprintf( current_parser_error, 100,
                        " { %s } "
                        " Defined externally and locally. ",
                        peeked_1->token );
                return -1;
            }

            break;
        default:
            current_parser_error = 
                realloc ( current_parser_error, 100 + strlen(peeked_1->token) );
            snprintf( current_parser_error, 100,
                    "Expected token TOK_IDENTIFIER found : %s",
                    peeked_1->token );
            return -1;
    }

    return EXIT_SUCCESS;
}


// TODO: make this parse the section "maybe"
// priority low 
int parseSection( program_t *program, token_t *c_tok ) 
{
    token_t *peeked_1 = getNextToken( program ), *peeked_2;
    bool defined;
    token_t *t;

    switch ( peeked_1->type ) 
    {
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
    token_t *t;

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

            if ( peeked_2->type == TOK_IDENTIFIER )
            {
                HASH_FIND_STR( program->table->tokens, peeked_2->token, t );
                if ( t == NULL )
                    goto not_defined;
                else 
                    insert( program->sections->dot_text, t->value );
            }else if ( peeked_2->type == TOK_LITERAL ||
                       peeked_2->type == TOK_LITERAL_HEX ) {
                    insert( program->sections->dot_text, peeked_2->value );
            }else 
            {
                current_parser_error = 
                    realloc ( current_parser_error, 100 + strlen(peeked_1->token) );
                sprintf( current_parser_error, 
                    "Expected token type ( TOK_LITERAL ||"
                    " TOK_IDENTIFIER || TOK_LITERAL_HEX )"
                    " found : %s", peeked_1->token );
                return -1;
            }
            break;
        case TOK_LITERAL:
        case TOK_LITERAL_HEX:
            insert( program->sections->dot_text, PUT_IMMEDIATE );
            insert( program->sections->dot_text, peeked_1->value );
            break;
        case TOK_IDENTIFIER:

            HASH_FIND_STR( program->table->tokens, peeked_1->token, t );
            if ( t == NULL )
            {
        not_defined:
                current_parser_error = 
                    realloc ( current_parser_error, 25 + strlen(peeked_1->token) );
                sprintf( current_parser_error, 
                        "Undefined identifier : %s", peeked_1->token );
                return -1;
            }else {
                    insert( program->sections->dot_text, PUT_DIRECT );
                    insert( program->sections->dot_text, t->value    );
            }
            break;
        default:
            current_parser_error = 
                realloc ( current_parser_error, 100 + strlen(peeked_1->token) );
            snprintf( current_parser_error, 100,
                    "Expected token ( TOK_ADDRESSING ||"
                    " TOK_IDENTIFIER || TOK_LITERAL"
                    " || TOK_LITERAL_HEX ) found : %s", peeked_1->token );
            return -1;
    }

    return EXIT_SUCCESS;
}

int parseRead( program_t *program, token_t *c_tok ) 
{
    token_t *peeked_1 = getNextToken( program ), *peeked_2;
    bool defined;
    token_t *t;

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

            if ( peeked_2->type == TOK_IDENTIFIER )
            {
                HASH_FIND_STR( program->table->tokens, peeked_2->token, t );
                if ( t == NULL )
                    goto not_defined;
                else 
                    insert( program->sections->dot_text, t->value );
            }else if ( peeked_2->type == TOK_LITERAL ||
                       peeked_2->type == TOK_LITERAL_HEX ) {
                    insert( program->sections->dot_text, peeked_2->value );
            }else 
            {
                current_parser_error = 
                    realloc ( current_parser_error, 100 + strlen(peeked_1->token) );
                sprintf( current_parser_error, 
                    "Expected token type ( TOK_LITERAL ||"
                    " TOK_IDENTIFIER || TOK_LITERAL_HEX )"
                    " found : %s", peeked_1->token );
                return -1;
            }
            break;
        case TOK_LITERAL:
        case TOK_LITERAL_HEX:
            insert( program->sections->dot_text, READ_DIRECT );
            insert( program->sections->dot_text, peeked_1->value );
            break;
        case TOK_IDENTIFIER:
            HASH_FIND_STR(program->table->tokens, peeked_1->token, t);
            if ( t == NULL )
            {
        not_defined:
                current_parser_error = 
                    realloc ( current_parser_error, 25 + strlen(peeked_1->token) );
                sprintf( current_parser_error, 
                        "Undefined identifier : %s", peeked_1->token );
                return -1;
            }else 
            {
                    insert( program->sections->dot_text, READ_DIRECT );
                    insert( program->sections->dot_text, t->value    );
            }
            break;
        default:
            current_parser_error = 
                realloc ( current_parser_error, 100 + strlen(peeked_1->token) );
            snprintf( current_parser_error, 100,
                    "Expected token ( TOK_ADDRESSING ||"
                    " TOK_IDENTIFIER || TOK_LITERAL"
                    " || TOK_LITERAL_HEX ) found : %s", peeked_1->token );
            return -1;
    }

    return EXIT_SUCCESS;
}

int parseBRPOS( program_t *program, token_t *c_tok ) 
{
    token_t *peeked_1 = getNextToken( program ), *peeked_2;
    bool defined;
    token_t *t;

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

            if ( peeked_2->type == TOK_IDENTIFIER )
            {
                HASH_FIND_STR( program->table->tokens, peeked_2->token, t );
                if ( t == NULL )
                    goto not_defined;
                else 
                    insert( program->sections->dot_text, t->value );
            }else if ( peeked_2->type == TOK_LITERAL ||
                       peeked_2->type == TOK_LITERAL_HEX ) {
                    insert( program->sections->dot_text, peeked_2->value );
            }else 
            {
                current_parser_error = 
                    realloc ( current_parser_error, 100 + strlen(peeked_1->token) );
                sprintf( current_parser_error, 
                    "Expected token type ( TOK_LITERAL ||"
                    " TOK_IDENTIFIER || TOK_LITERAL_HEX )"
                    " found : %s", peeked_1->token );
                return -1;
            }
            break;
        case TOK_LITERAL:
        case TOK_LITERAL_HEX:
            insert( program->sections->dot_text, BRPOS_IMMEDIATE );
            insert( program->sections->dot_text, peeked_1->value );
            break;
        case TOK_IDENTIFIER:
            HASH_FIND_STR( program->table->tokens, peeked_1->token, t );
            if ( t == NULL )
            {
        not_defined:
                current_parser_error = 
                    realloc ( current_parser_error, 25 + strlen(peeked_1->token) );
                sprintf( current_parser_error, 
                        "Undefined identifier : %s", peeked_1->token );
                return -1;
            }else {
                    insert( program->sections->dot_text, BRPOS_IMMEDIATE );
                    insert( program->sections->dot_text, t->value        );
            }

            break;
        default:
            current_parser_error = 
                realloc ( current_parser_error, 100 + strlen(peeked_1->token) );
            snprintf( current_parser_error, 100,
                    "Expected token ( TOK_ADDRESSING ||"
                    " TOK_IDENTIFIER || TOK_LITERAL"
                    " || TOK_LITERAL_HEX ) found : %s", peeked_1->token );
            return -1;
    }

    return EXIT_SUCCESS;
}

int parseBRZERO( program_t *program, token_t *c_tok ) 
{
    token_t *peeked_1 = getNextToken( program ), *peeked_2;
    bool defined;
    token_t *t;

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

            if ( peeked_2->type == TOK_IDENTIFIER )
            {
                HASH_FIND_STR( program->table->tokens, peeked_2->token, t );
                if ( t == NULL )
                    goto not_defined;
                else 
                    insert( program->sections->dot_text, t->value );
            }else if ( peeked_2->type == TOK_LITERAL ||
                       peeked_2->type == TOK_LITERAL_HEX ) {
                    insert( program->sections->dot_text, peeked_2->value );
            }else 
            {
                current_parser_error = 
                    realloc ( current_parser_error, 100 + strlen(peeked_1->token) );
                sprintf( current_parser_error, 
                    "Expected token type ( TOK_LITERAL ||"
                    " TOK_IDENTIFIER || TOK_LITERAL_HEX )"
                    " found : %s", peeked_1->token );
                return -1;
            }
            break;
        case TOK_LITERAL:
        case TOK_LITERAL_HEX:
            insert( program->sections->dot_text, BRZERO_IMMEDIATE    );
            insert( program->sections->dot_text, peeked_1->value );
            break;
        case TOK_IDENTIFIER:
            HASH_FIND_STR( program->table->tokens, peeked_1->token, t);
            if ( t == NULL )
            {
        not_defined:
                current_parser_error = 
                    realloc ( current_parser_error, 25 + strlen(peeked_1->token) );
                sprintf( current_parser_error, 
                        "Undefined identifier : %s", peeked_1->token );
                return -1;
            }else 
            {
                    insert( program->sections->dot_text, BRZERO_IMMEDIATE );
                    insert( program->sections->dot_text, t->value         );
            }
            break;
        default:
            current_parser_error = 
                realloc ( current_parser_error, 100 + strlen(peeked_1->token) );
            snprintf( current_parser_error, 100,
                    "Expected token ( TOK_ADDRESSING ||"
                    " TOK_IDENTIFIER || TOK_LITERAL"
                    " || TOK_LITERAL_HEX ) found : %s", peeked_1->token );
            return -1;
    }

    return EXIT_SUCCESS;
}

int parseBRNEG( program_t *program, token_t *c_tok ) 
{
    token_t *peeked_1 = getNextToken( program ), *peeked_2;
    bool defined;
    token_t *t;

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

            if ( peeked_2->type == TOK_IDENTIFIER )
            {
                HASH_FIND_STR( program->table->tokens, peeked_2->token, t );
                if ( t == NULL )
                    goto not_defined;
                else 
                    insert( program->sections->dot_text, t->value );
            }else if ( peeked_2->type == TOK_LITERAL ||
                       peeked_2->type == TOK_LITERAL_HEX ) {
                    insert( program->sections->dot_text, peeked_2->value );
            }else 
            {
                current_parser_error = 
                    realloc ( current_parser_error, 100 + strlen(peeked_1->token) );
                sprintf( current_parser_error, 
                    "Expected token type ( TOK_LITERAL ||"
                    " TOK_IDENTIFIER || TOK_LITERAL_HEX )"
                    " found : %s", peeked_1->token );
                return -1;
            }
            break;
        case TOK_LITERAL:
        case TOK_LITERAL_HEX:
            insert( program->sections->dot_text, BRNEG_IMMEDIATE    );
            insert( program->sections->dot_text, peeked_1->value );
            break;
        case TOK_IDENTIFIER:
            HASH_FIND_STR( program->table->tokens, peeked_1->token, t); 
            if ( t == NULL )
            {
        not_defined:
                current_parser_error = 
                    realloc ( current_parser_error, 25 + strlen(peeked_1->token) );
                sprintf( current_parser_error, 
                        "Undefined identifier : %s", peeked_1->token );
                return -1;
            }else 
            {
                    insert( program->sections->dot_text, BRNEG_IMMEDIATE );
                    insert( program->sections->dot_text, t->value        );
            }
            break;
        default:
            current_parser_error = 
                realloc ( current_parser_error, 100 + strlen(peeked_1->token) );
            snprintf( current_parser_error, 100,
                    "Expected token ( TOK_ADDRESSING ||"
                    " TOK_IDENTIFIER || TOK_LITERAL"
                    " || TOK_LITERAL_HEX ) found : %s", peeked_1->token );
            return -1;
    }

    return EXIT_SUCCESS;
}

int parseBR( program_t *program, token_t *c_tok ) 
{
    token_t *peeked_1 = getNextToken( program ), *peeked_2;
    bool defined;
    token_t *t;

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

            if ( peeked_2->type == TOK_IDENTIFIER )
            {
                HASH_FIND_STR( program->table->tokens, peeked_2->token, t );
                if ( t == NULL )
                    goto not_defined;
                else 
                    insert( program->sections->dot_text, t->value );
            }else if ( peeked_2->type == TOK_LITERAL ||
                       peeked_2->type == TOK_LITERAL_HEX ) {
                    insert( program->sections->dot_text, peeked_2->value );
            }else 
            {
                current_parser_error = 
                    realloc ( current_parser_error, 100 + strlen(peeked_1->token) );
                sprintf( current_parser_error, 
                    "Expected token type ( TOK_LITERAL ||"
                    " TOK_IDENTIFIER || TOK_LITERAL_HEX )"
                    " found : %s", peeked_1->token );
                return -1;
            }
            break;
        case TOK_LITERAL:
        case TOK_LITERAL_HEX:
            insert( program->sections->dot_text, BR_IMMEDIATE    );
            insert( program->sections->dot_text, peeked_1->value );
            break;
        case TOK_IDENTIFIER:
            HASH_FIND_STR( program->table->tokens, peeked_1->token, t); 
            if ( t == NULL )
            {
        not_defined:
                current_parser_error = 
                    realloc ( current_parser_error, 25 + strlen(peeked_1->token) );
                sprintf( current_parser_error, 
                        "Undefined identifier : %s", peeked_1->token );
                return -1;
            }else 
            {
                    insert( program->sections->dot_text, BR_IMMEDIATE );
                    insert( program->sections->dot_text, t->value     );
            }

            break;
        default:
            current_parser_error = 
                realloc ( current_parser_error, 100 + strlen(peeked_1->token) );
            snprintf( current_parser_error, 100,
                    "Expected token ( TOK_ADDRESSING ||"
                    " TOK_IDENTIFIER || TOK_LITERAL"
                    " || TOK_LITERAL_HEX ) found : %s", peeked_1->token );
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
    token_t *t;

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

            if ( peeked_2->type == TOK_IDENTIFIER )
            {
                HASH_FIND_STR( program->table->tokens, peeked_2->token, t );
                if ( t == NULL )
                    goto not_defined;
                else 
                    insert( program->sections->dot_text, t->value );
            }else if ( peeked_2->type == TOK_LITERAL ||
                       peeked_2->type == TOK_LITERAL_HEX ) {
                    insert( program->sections->dot_text, peeked_2->value );
            }else 
            {
                current_parser_error = 
                    realloc ( current_parser_error, 100 + strlen(peeked_1->token) );
                sprintf( current_parser_error, 
                    "Expected token type ( TOK_LITERAL ||"
                    " TOK_IDENTIFIER || TOK_LITERAL_HEX )"
                    " found : %s", peeked_1->token );
                return -1;
            }

            break;
        case TOK_LITERAL:
        case TOK_LITERAL_HEX:
            insert( program->sections->dot_text, MULT_IMMEDIATE );
            insert( program->sections->dot_text, peeked_1->value );
            break;
        case TOK_IDENTIFIER:
            HASH_FIND_STR(program->table->tokens, peeked_1->token, t); 
            if ( t == NULL )
            {
        not_defined:
                current_parser_error = 
                    realloc ( current_parser_error, 25 + strlen(peeked_1->token) );
                sprintf( current_parser_error, 
                        "Undefined identifier : %s", peeked_1->token );
                return -1;
            }else
            {
                    insert( program->sections->dot_text, MULT_DIRECT );
                    insert( program->sections->dot_text, t->value    );
            }
            break;
        default:
            current_parser_error = 
                realloc ( current_parser_error, 100 + strlen(peeked_1->token) );
            snprintf( current_parser_error, 100,
                    "Expected token ( TOK_ADDRESSING ||"
                    " TOK_IDENTIFIER || TOK_LITERAL"
                    " || TOK_LITERAL_HEX ) found : %s", peeked_1->token );
            return -1;
    }

    return EXIT_SUCCESS;
}

int parseSUB( program_t *program, token_t *c_tok ) 
{
    token_t *peeked_1 = getNextToken( program ), *peeked_2;
    bool defined;
    token_t *t;

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

            if ( peeked_2->type == TOK_IDENTIFIER )
            {
                HASH_FIND_STR( program->table->tokens, peeked_2->token, t );
                if ( t == NULL )
                    goto not_defined;
                else 
                    insert( program->sections->dot_text, t->value );
            }else if ( peeked_2->type == TOK_LITERAL ||
                       peeked_2->type == TOK_LITERAL_HEX ) {
                    insert( program->sections->dot_text, peeked_2->value );
            }else 
            {
                current_parser_error = 
                    realloc ( current_parser_error, 100 + strlen(peeked_1->token) );
                sprintf( current_parser_error, 
                    "Expected token type ( TOK_LITERAL ||"
                    " TOK_IDENTIFIER || TOK_LITERAL_HEX )"
                    " found : %s", peeked_1->token );
                return -1;
            }
            break;
        case TOK_LITERAL:
        case TOK_LITERAL_HEX:
            insert( program->sections->dot_text, SUB_IMMEDIATE );
            insert( program->sections->dot_text, peeked_1->value );
            break;
        case TOK_IDENTIFIER:
            HASH_FIND_STR( program->table->tokens, peeked_1->token, t );
            if ( t == NULL )
            {
        not_defined:
                current_parser_error = 
                    realloc ( current_parser_error, 25 + strlen(peeked_1->token) );
                sprintf( current_parser_error, 
                        "Undefined identifier : %s", peeked_1->token );
                return -1;
            }else
            {
                    insert( program->sections->dot_text, SUB_DIRECT );
                    insert( program->sections->dot_text, t->value   );
            }

            break;
        default:
            current_parser_error = 
                realloc ( current_parser_error, 100 + strlen(peeked_1->token) );
            snprintf( current_parser_error, 100,
                    "Expected token ( TOK_ADDRESSING ||"
                    " TOK_IDENTIFIER || TOK_LITERAL"
                    " || TOK_LITERAL_HEX ) found : %s", peeked_1->token );
            return -1;
    }

    return EXIT_SUCCESS;
}

int parseDIVIDE( program_t *program, token_t *c_tok ) 
{
    token_t *peeked_1 = getNextToken( program ), *peeked_2;
    bool defined;
    token_t *t;

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

            if ( peeked_2->type == TOK_IDENTIFIER )
            {
                HASH_FIND_STR( program->table->tokens, peeked_2->token, t );
                if ( t == NULL )
                    goto not_defined;
                else 
                    insert( program->sections->dot_text, t->value );
            }else if ( peeked_2->type == TOK_LITERAL ||
                       peeked_2->type == TOK_LITERAL_HEX ) {
                    insert( program->sections->dot_text, peeked_2->value );
            }else 
            {
                current_parser_error = 
                    realloc ( current_parser_error, 100 + strlen(peeked_1->token) );
                sprintf( current_parser_error, 
                    "Expected token type ( TOK_LITERAL ||"
                    " TOK_IDENTIFIER || TOK_LITERAL_HEX )"
                    " found : %s", peeked_1->token );
                return -1;
            }
            break;
        case TOK_LITERAL:
        case TOK_LITERAL_HEX:
            insert( program->sections->dot_text, DIV_IMMEDIATE );
            insert( program->sections->dot_text, peeked_1->value );
            break;
        case TOK_IDENTIFIER:
            HASH_FIND_STR( program->table->tokens, peeked_1->token, t );
            if ( t == NULL )
            {
        not_defined:
                current_parser_error = 
                    realloc ( current_parser_error, 25 + strlen(peeked_1->token) );
                sprintf( current_parser_error, 
                        "Undefined identifier : %s", peeked_1->token );
                return -1;
            }else
            {
                    insert( program->sections->dot_text, DIV_DIRECT );
                    insert( program->sections->dot_text, t->value   );
            }
            break;
        default:
            current_parser_error = 
                realloc ( current_parser_error, 100 + strlen(peeked_1->token) );
            snprintf( current_parser_error, 100,
                    "Expected token ( TOK_ADDRESSING ||"
                    " TOK_IDENTIFIER || TOK_LITERAL"
                    " || TOK_LITERAL_HEX ) found : %s", peeked_1->token );
            return -1;
    }

    return EXIT_SUCCESS;
}

int parseADD( program_t *program, token_t *c_tok ) 
{
    token_t *peeked_1 = getNextToken( program ), *peeked_2;
    bool defined;
    token_t *t;

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

            if ( peeked_2->type == TOK_IDENTIFIER )
            {
                HASH_FIND_STR( program->table->tokens, peeked_2->token, t );
                if ( t == NULL )
                    goto not_defined;
                else 
                    insert( program->sections->dot_text, t->value );
            }else if ( peeked_2->type == TOK_LITERAL ||
                       peeked_2->type == TOK_LITERAL_HEX ) {
                    insert( program->sections->dot_text, peeked_2->value );
            }else 
            {
                current_parser_error = 
                    realloc ( current_parser_error, 100 + strlen(peeked_1->token) );
                sprintf( current_parser_error, 
                    "Expected token type ( TOK_LITERAL ||"
                    " TOK_IDENTIFIER || TOK_LITERAL_HEX )"
                    " found : %s", peeked_1->token );
                return -1;
            }
            break;
        case TOK_LITERAL:
        case TOK_LITERAL_HEX:
            insert( program->sections->dot_text, ADD_IMMEDIATE );
            insert( program->sections->dot_text, peeked_1->value );
            break;
        case TOK_IDENTIFIER:
            HASH_FIND_STR( program->table->tokens, peeked_1->token, t );
            if ( t == NULL )
            {
        not_defined:
                current_parser_error = 
                    realloc ( current_parser_error, 25 + strlen(peeked_1->token) );
                sprintf( current_parser_error, 
                        "Undefined identifier : %s", peeked_1->token );
                return -1;
            }else 
            {
                    insert( program->sections->dot_text, ADD_DIRECT );
                    insert( program->sections->dot_text, t->value   );
            }
            break;
        default:
            current_parser_error = 
                realloc ( current_parser_error, 100 + strlen(peeked_1->token) );
            snprintf( current_parser_error, 100,
                    "Expected token ( TOK_ADDRESSING ||"
                    " TOK_IDENTIFIER || TOK_LITERAL"
                    " || TOK_LITERAL_HEX ) found : %s", peeked_1->token );
            return -1;
    }

    return EXIT_SUCCESS;
}

int parseLoad( program_t *program, token_t *c_tok ) 
{
    token_t *peeked_1 = getNextToken( program ), *peeked_2;
    bool defined;
    token_t *t;

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

            if ( peeked_2->type == TOK_IDENTIFIER )
            {
                HASH_FIND_STR( program->table->tokens, peeked_2->token, t );
                if ( t == NULL )
                    goto not_defined;
                else 
                    insert( program->sections->dot_text, t->value );
            }else if ( peeked_2->type == TOK_LITERAL ||
                       peeked_2->type == TOK_LITERAL_HEX ) {
                    insert( program->sections->dot_text, peeked_2->value );
            }else 
            {
                current_parser_error = 
                    realloc ( current_parser_error, 100 + strlen(peeked_1->token) );
                sprintf( current_parser_error, 
                    "Expected token ( TOK_LITERAL ||"
                    " TOK_IDENTIFIER || TOK_LITERAL_HEX )"
                    " found : %s", peeked_1->token );
                return -1;
            }
            break;
        case TOK_LITERAL:
        case TOK_LITERAL_HEX:
            insert( program->sections->dot_text, LOAD_IMMEDIATE  );
            insert( program->sections->dot_text, peeked_1->value );
            break;
        case TOK_IDENTIFIER:
            HASH_FIND_STR( program->table->tokens, peeked_1->token, t );
            if ( t == NULL )
            {
        not_defined:
                current_parser_error = 
                    realloc ( current_parser_error, 25 + strlen(peeked_1->token) );
                sprintf( current_parser_error, 
                        "Undefined identifier : %s", peeked_1->token );
                return -1;
            }else {
                    insert( program->sections->dot_text, LOAD_DIRECT );
                    insert( program->sections->dot_text, t->value    );
            }
            break;
        default:
              current_parser_error = 
                realloc ( current_parser_error, 100 + strlen(peeked_1->token) );
            snprintf( current_parser_error, 100,
                    "Expected token ( TOK_ADDRESSING ||"
                    " TOK_IDENTIFIER || TOK_LITERAL"
                    " || TOK_LITERAL_HEX ) found : %s", peeked_1->token );
            return -1;
    }

    return EXIT_SUCCESS;
}

int parseStore( program_t *program, token_t *c_tok ) 
{
    token_t *peeked_1 = getNextToken( program ), *peeked_2;
    bool defined;
    token_t *t;

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
                HASH_FIND_STR( program->table->tokens, peeked_2->token, t );
                if ( t == NULL )
                    goto not_defined;
                else 
                    insert( program->sections->dot_text, t->value );
            }else if ( peeked_2->type == TOK_LITERAL ||
                       peeked_2->type == TOK_LITERAL_HEX ) {
                    insert( program->sections->dot_text, peeked_2->value );
            }else 
            {
                current_parser_error = 
                    realloc ( current_parser_error, 100 + strlen(peeked_1->token) );
                sprintf( current_parser_error, 
                    "Expected token type ( TOK_LITERAL ||"
                    " TOK_IDENTIFIER || TOK_LITERAL_HEX )"
                    " found : %s", peeked_1->token );
                return -1;
            }
            break;
        case TOK_LITERAL:
        case TOK_LITERAL_HEX:
            insert( program->sections->dot_text, STORE_DIRECT );
            insert( program->sections->dot_text, peeked_1->value );
            break;
        case TOK_IDENTIFIER:
            HASH_FIND_STR( program->table->tokens, peeked_1->token, t );
            if ( t == NULL )
            {
        not_defined:
                current_parser_error = 
                    realloc ( current_parser_error, 25 + strlen(peeked_1->token) );
                sprintf( current_parser_error, 
                        "Undefined identifier : %s", peeked_1->token );
                return -1;
            }else {
                    insert( program->sections->dot_text, STORE_DIRECT );
                    insert( program->sections->dot_text, t->value     );
            }
        break;
        default:
            current_parser_error = 
                realloc ( current_parser_error, 100 + strlen(peeked_1->token) );
            snprintf( current_parser_error, 100,
                    "Expected token ( TOK_ADDRESSING ||"
                    " TOK_IDENTIFIER || TOK_LITERAL"
                    " || TOK_LITERAL_HEX ) found : %s", peeked_1->token );
            return -1;
            break;
    }

    return EXIT_SUCCESS;
}

