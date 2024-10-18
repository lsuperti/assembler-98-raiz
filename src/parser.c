
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
        case TOK_ADDRESSING: //caso tenha token definindo enderecamento
            peeked_2 = getNextToken( program );
            if ( strncmp( peeked_1->token, "&", 2 ) == 0 ) //direto
            {
                insert( program->sections->dot_text, LOAD_DIRECT  );
            }else 
            {
                insert( program->sections->dot_text, LOAD_INDIRECT ); //indireto
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
        case TOK_LITERAL: //imediato
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

enum copy_address{
    UNDEFINED,
    DIRECT,
    INDIRECT,
    IMMEDIATE
};

int parseCopy(program_t *program, token_t *c_tok){
    // apos deteccao do token copy, obtem proximo token
    token_t *peeked_1, *peeked_2; //para percorrer os tokens
    bool defined;
    token_t *t; //para procurar na tabela de simbolos

    enum copy_address op1_type = UNDEFINED, op2_type = UNDEFINED;
    word_t op1_value, op2_value;
    bool op2_defined = false; //ate resolver o operando 2

    do{

        peeked_1 = getNextToken(program);

        switch (peeked_1->type){

            case TOK_ADDRESSING: //caso tenha token definindo enderecamento
                peeked_2 = getNextToken( program );
                if ( strncmp( peeked_1->token, "&", 2 ) == 0 ){ //direto: simbolo &
                    //insert( program->sections->dot_text, LOAD_DIRECT  );
                    if(op1_type == UNDEFINED){
                        op1_type = DIRECT;
                    } else {
                        op2_type = DIRECT;
                    }
                }else // indireto: simbolo #
                {
                    //insert( program->sections->dot_text, LOAD_INDIRECT );
                    if(op1_type == UNDEFINED){
                        op1_type = INDIRECT;
                    } else {
                        op2_type = INDIRECT;
                    }
                }

                // proximo token: identificador ou literal
                if (peeked_2->type == TOK_IDENTIFIER){ //identificador

                    HASH_FIND_STR(program->table->tokens, peeked_2->token, t);
                    
                    if ( t == NULL ){ //se nao encontrado, aponta erro
                        goto not_defined;
                    } else {
                        //insert( program->sections->dot_text, t->value );
                        if(op2_type == UNDEFINED){
                            op1_value = t->value;
                        } else {
                            op2_value = t->value;
                            op2_defined = true;
                        }
                    }
                } else if ( peeked_2->type == TOK_LITERAL ||
                    peeked_2->type == TOK_LITERAL_HEX ){ //literal
                    //insert( program->sections->dot_text, peeked_2->value );
                    if(op2_type == UNDEFINED){
                            op1_value = peeked_2->value;
                        } else {
                            op2_value = peeked_2->value;
                            op2_defined = true;
                        }
                } else { //caso token inesperado, aponta erro
                
                    current_parser_error = 
                        realloc ( current_parser_error, 100 + strlen(peeked_1->token) );
                    sprintf( current_parser_error, 
                        "Expected token ( TOK_LITERAL ||"
                        " TOK_IDENTIFIER || TOK_LITERAL_HEX )"
                        " found : %s", peeked_1->token );
                    return -1;
                }
                break;
            
            case TOK_LITERAL: //imediato
            case TOK_LITERAL_HEX:
                //insert( program->sections->dot_text, LOAD_IMMEDIATE  );
                //insert( program->sections->dot_text, peeked_1->value );
                if(op1_type == UNDEFINED){
                    //op1_type = IMMEDIATE;
                    //op1_value = peeked_1->value;
                    // operando 1 nao pode ser imediato, entao aponta erro
                    current_parser_error = realloc (current_parser_error, 100 + strlen(peeked_1->token));
                    snprintf( current_parser_error, 100,
                        "Expected token (TOK_ADDRESSING ||"
                        " TOK_IDENTIFIER) for first operand"
                        " in COPY instruction. Found : %s", peeked_1->token );
                    return -1;
                } else {
                    op2_type = IMMEDIATE;
                    op2_value = peeked_1->value;
                    op2_defined = true;
                }
                break;
            
            case TOK_IDENTIFIER: //identificador
                HASH_FIND_STR( program->table->tokens, peeked_1->token, t );
                if ( t == NULL )
                {
            not_defined:
                    current_parser_error = 
                        realloc ( current_parser_error, 25 + strlen(peeked_1->token) );
                    sprintf( current_parser_error, 
                            "Undefined identifier : %s", peeked_1->token );
                    return -1;
                } else {
                    //insert( program->sections->dot_text, LOAD_DIRECT );
                    //insert( program->sections->dot_text, t->value    );
                    if(op1_type == UNDEFINED){
                        op1_type = DIRECT;
                        op1_value = t->value;
                    } else {
                        op2_type = DIRECT;
                        op2_value = t->value;
                        op2_defined = true;
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
    }while(op2_defined == false);

    //temporario
    if(op1_type == IMMEDIATE){ return -1; }

    // depois de analisado, insere a instrucao
    switch (op1_type){
        case DIRECT:
            switch (op2_type) {
                case DIRECT:        // DIRECT vs DIRECT
                    insert( program->sections->dot_text, COPY_DD );
                    break;
                case INDIRECT:      // DIRECT vs INDIRECT
                    insert( program->sections->dot_text, COPY_DI );
                    break;
                case IMMEDIATE:     // DIRECT vs IMMEDIATE
                    insert( program->sections->dot_text, COPY_DIm );
                    break;
            }
            break;

        case INDIRECT:
            switch (op2_type) {
                case DIRECT:        // INDIRECT vs DIRECT
                    insert( program->sections->dot_text, COPY_ID );
                    break;
                case INDIRECT:      // INDIRECT vs INDIRECT
                    insert( program->sections->dot_text, COPY_II );
                    break;
                case IMMEDIATE:     // INDIRECT vs IMMEDIATE
                    insert( program->sections->dot_text, COPY_IIm );
                    break;
            }
            break;
    }
    
    //apos resolver a instrucao, insere os 2 operandos
    insert(program->sections->dot_text, op1_value);
    insert(program->sections->dot_text, op2_value);

    return EXIT_SUCCESS;
}