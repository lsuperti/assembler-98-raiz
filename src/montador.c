
#include "montador.h"
#include <ctype.h>

#define WHITESPACES " \n\t\r"

/* 
 * Arquivo com código source é passado para 
 * essa função.
*/
program_t* createProgram( FILE *file )
{ 
    assert( file != NULL );
    program_t *program =
    ( program_t * ) malloc( sizeof ( program_t ) );

    long lSize;
    char *buffer;

    fseek( file, 0L , SEEK_END);
    lSize = ftell( file );
    rewind( file );

    buffer = calloc( 1, lSize+1 );
    if( !buffer ) fclose(file),fputs("Memory alloc fails",stderr),exit(1);

    if( 1!=fread( buffer, lSize, 1, file) )
      fclose(file),free(buffer),fputs("Entire read fails",stderr),exit(1);

    program->source       = buffer;
    program->HEAD         = 0;             
    program->tokens       = NULL;
    program->n_tokens     = 0;
    program->sections     = NULL;
    program->table        = NULL;
    program->program_size = strlen(program->source);

    return program;
}

/*
 * Libera a memoria da struct program_t.
*/
void freeProgram( program_t *program )
{
   if ( program->source )
       free(program->source);
   if ( program->sections ) {
       if ( program->sections->dot_text )
           free(program->sections->dot_text);
       if ( program->sections->dot_data )
           free(program->sections->dot_data);
       if ( program->sections->dot_rodata)
           free(program->sections->dot_rodata);
       free(program->sections);
       program->sections = NULL;
   }
   if ( program->table ) {
       if ( program->table->tokens )
           free( program->table->tokens );
       free(program->table);
       program->table = NULL;
   }
   if ( program->tokens ) {
       if ( program->tokens->token ) 
           free(program->tokens->token);
       free(program->tokens);
       program->tokens = NULL;
   }
   free(program);
   program = NULL;
}

void tokenize( program_t *program )
{ 
    token_t *tokens =
        ( token_t * ) malloc ( sizeof ( token_t ) );
    assert(tokens != NULL);
    token_t tok;
    int idx = 0;
    do { 
        tok = nextToken(program);
        if ( tok.token != NULL ) {
            tokens[idx++] = tok;
        }
        tokens = realloc( tokens, sizeof(token_t)*( idx + 1 ) );
        assert(tokens != NULL);
    }while ( tok.token != NULL );
    if ( idx != 0 ) {
        program->tokens   = tokens;
        program->n_tokens = ++idx;
    }
}

/* Peeks for next character also checks if possible 
 * returns empty if no next character since the 
 * character cannot be empty afterwards.
*/
char peek( char *str, int head )
{
   if ( head + 1 <= strlen(str) )
   {
       return str[head + 1];
   }
   return ' ';
}

/* Retorna um token NULL se não existem mais tokens */
/* Exemplo tokens válidos : 
 *      LOAD 
 *      203 
 *      RET
*/
token_t nextToken( program_t *program ) { 

    token_t *token_n = (token_t *) malloc ( sizeof ( token_t ) );
    assert( token_n != NULL );
    token_n->token = NULL;

    // Pula espaços em branco
    while ( program->HEAD < program->program_size && 
            strchr(WHITESPACES, program->source[program->HEAD]) )
    {
        program->HEAD++;
    }

    char *temp = ( char * ) malloc ( 1 );
    assert ( temp != NULL );
    temp[0] = '\0';
    int c = 0;
    bool digit = false;

    // Retorna token = numero
    while ( program->HEAD < program->program_size && 
         isdigit(program->source[program->HEAD]) )
    {
       digit = true;
       temp = ( char * ) realloc ( temp, c + 2 );
       assert( temp != NULL );
       temp[c] = program->source[program->HEAD]; 
       c++;
       program->HEAD++;
       temp[c] = '\0';
    }
    
    // Fazer digitos em hexa ( como mencionado no trabalho )
    if ( digit ) 
    {
        token_n->token   = temp;
        token_n->defined = true;
        token_n->value   = atoi(temp);
        token_n->type    = TOK_LITERAL;
        return *token_n;
    }

    // Fazer para todas as instruções  
    // Lembrando que para LOAD apenas a instrução em 
    // mnemonico mais alto nível é necessaria 
    switch ( program->source[program->HEAD++] ) {
        case '&':
           token_n->token   = "&";
           token_n->defined = true;
           token_n->value   = -1;
           token_n->type    = TOK_ADDRESSING;
            break;
        case '#':
           token_n->token   = "#"; 
           token_n->defined = true;
           token_n->value   = -1;
           token_n->type    = TOK_ADDRESSING;
            break;
        // -1 to +1 por que HEAD + 1 já foi feito
        // antes de entrar no caso.
        case 'L':
            if ( peek(program->source, program->HEAD - 1) 
                 == 'O'
                 && peek(program->source, program->HEAD)
                 == 'A'
                 && peek(program->source, program->HEAD + 1) 
                 == 'D' )
            {
                token_n->token   = "LOAD";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_INSTRUCTION;
                program->HEAD    += 3;
            }
        break;
        case 'S':
            if ( peek(program->source, program->HEAD - 1) 
                 == 'T'
                 && peek(program->source, program->HEAD)
                 == 'O'
                 && peek(program->source, program->HEAD + 1) 
                 == 'R' 
                 && peek(program->source, program->HEAD + 2) 
                 == 'E')
            {
                token_n->token   = "STORE";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_INSTRUCTION;
                program->HEAD    += 4;
            }      //S 
            else if( peek(program->source, program->HEAD - 1)
                 == 'T'
                 && peek(program->source, program->HEAD)
                 == 'O'
                 && peek(program->source, program->HEAD + 1) 
                 == 'P')
            {
                token_n->token   = "STOP";
                token_n->defined = false;
                token_n->value   = 11;
                token_n->type    = TOK_STOP;
                program->HEAD    += 3;
            }      //S
            else if(peek(program->source, program->HEAD - 1)
                 == 'U'
                 && peek(program->source, program->HEAD)
                 == 'B')
            {
                token_n->token   = "SUB";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_INSTRUCTION;
                program->HEAD    += 2;
            }
            //fiquei na duvida se incluir SPACE eh boa ideia
            // else if (peek(program->source, program->HEAD - 1) 
            //      == 'P'
            //      && peek(program->source, program->HEAD)
            //      == 'A'
            //      && peek(program->source, program->HEAD + 1) 
            //      == 'C' 
            //      && peek(program->source, program->HEAD + 2) 
            //      == 'E')
            // {
            //     token_n->token   = "SPACE";
            //     token_n->defined = false;
            //     token_n->value   = -1;
            //     token_n->type    = INSTRUCTION; //como identifica uma pseudoinstrucao nessa estrutura?
            //     program->HEAD    += 4;
            // }
                   //S
            else if (peek(program->source, program->HEAD - 1) 
                 == 'T'
                 && peek(program->source, program->HEAD)
                 == 'A'
                 && peek(program->source, program->HEAD + 1) 
                 == 'C' 
                 && peek(program->source, program->HEAD + 2) 
                 == 'K')
            {
                token_n->token   = "STACK";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_STACK; //como identifica uma pseudoinstrucao nessa estrutura?
                program->HEAD    += 4;
            }
        break;
        case 's':
            if ( peek(program->source, program->HEAD - 1) 
                 == 'e'
                 && peek(program->source, program->HEAD)
                 == 'c'
                 && peek(program->source, program->HEAD + 1) 
                 == 't' 
                 && peek(program->source, program->HEAD + 2) 
                 == 'i' 
                 && peek(program->source, program->HEAD + 3) 
                 == 'o'
                 && peek(program->source, program->HEAD + 4) 
                 == 'n' )
            {
                token_n->token   = "section";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_SECTION;
                program->HEAD    += 6;
            }
        break;
        // vvvv Fazer IDENTIFICADORES 
        default:
            break;
    }

    return *token_n;
}

token_t *peekToken( program_t *program, int head )
{
    if ( head + 1 < program->n_tokens ) 
    { 
        return &program->tokens[head + 1];
    }else 
    {
        return NULL;
    }
}

void parse( program_t *program ) 
{
    assert( program->tokens != NULL );
    bool section_hit = false;

    int sec_n = 0, text_idx, data_idx; 

    // Sanity checks before parsing
    // and symbol table generation
    for ( int k=0; k < program->n_tokens; k++ ) 
    {
        token_t tok = program->tokens[k];
        token_t *peeked;
        switch ( tok.type ) 
        {
            case TOK_SECTION:
                ++sec_n;
                peeked = peekToken(program, k);
                assert( peeked != NULL );
                if 
                ( strcmp( peeked->token, ".text" ) )
                { 
                    text_idx = k+2;
                }else if
                ( strcmp( peeked->token, ".data" ) )
                {
                    data_idx = k+2;
                }else
                {
                    // Error undefined section.
                    return;
                }
                break;
            case TOK_IDENTIFIER:
                if ( 0 < k ) 
                {
                    if ( program->tokens[ k - 1 ].type != TOK_GLOBAL &&
                         program->tokens[ k - 1 ].type != TOK_EXTERN )
                    {
                    }
                }


                break;
            case TOK_GLOBAL:
            case TOK_EXTERN:
                peeked = peekToken(program, k);
                if ( peeked->type != TOK_IDENTIFIER ) 
                {
                    // Error : Expected identifier found something else
                    return;
                }
                break;
            case TOK_UNKNOWN:
                // Error undefined token
                return;
                break;
            default:
                break;
        }
    }

    if ( sec_n != 2 )
    {
        // Error more than two sections.
        return;
    }

    // Parse text section
    for( int i=text_idx; i < program->n_tokens; i++ )
    {
        token_t tok = program->tokens[i];
        

    }

    // Parse data section
    for( int i=data_idx; i < program->n_tokens; i++ )
    {
        token_t tok = program->tokens[i];
        

    }
}

