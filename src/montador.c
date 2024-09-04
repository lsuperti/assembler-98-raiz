
#include "montador.h"

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
    program->token_idx    = 0;
    program->n_tokens     = 0;
    program->sections     = NULL;
    program->table        = NULL;
    program->program_size = strlen(program->source);
    program->globals      = NULL;
    program->externs      = NULL;

    return program;
}

/*
 * Libera a memoria da struct program_t.
*/
void freeProgram( program_t *program )
{
   if ( !program ) return;
   if ( program->source )
   {
       free(program->source);
       program->source = NULL;
   }
   if ( program->sections ) {
       free(program->sections);
       program->sections = NULL;
   }
   if ( program->table ) {
       free(program->table);
       program->table = NULL;
   }
   if ( program->tokens ) {
       free(program->tokens);
       program->tokens = NULL;
   }
   if ( program->globals ) {
       free(program->globals);
       program->globals = NULL;
   }
   if ( program->externs ) {
       free(program->externs);
       program->externs = NULL;
   }
   if ( program ) {
       free(program);
       program = NULL;
   }
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

    temp = ( char * ) malloc ( 1 );
    assert ( temp != NULL );
    temp[0] = '\0';
    c = 0;

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
                token_n->type    = TOK_LOAD;
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
                token_n->type    = TOK_STORE;
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
                token_n->type    = TOK_SUB;
                program->HEAD    += 2;
            }
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
                token_n->defined = true;
                token_n->value   = -1;
                token_n->type    = TOK_STACK;
                program->HEAD    += 4;
            }
        break;
        case 'B':
            if(peek(program->source, program->HEAD - 1) 
                 == 'R'
                 && peek(program->source, program->HEAD)
                 == 'Z'
                 && peek(program->source, program->HEAD + 1) 
                 == 'E' 
                 && peek(program->source, program->HEAD + 2) 
                 == 'R'
                 && peek(program->source, program->HEAD + 3) 
                 == 'O')
            {
                token_n->token   = "BRZERO";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_BRZERO;
                program->HEAD    += 5;
            }      //B
            else if(peek(program->source, program->HEAD - 1) 
                 == 'R'
                 && peek(program->source, program->HEAD)
                 == 'P'
                 && peek(program->source, program->HEAD + 1) 
                 == 'O' 
                 && peek(program->source, program->HEAD + 2) 
                 == 'S')
            {
                token_n->token   = "BRPOS";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_BRPOS;
                program->HEAD    += 4;
            }      //B
            else if(peek(program->source, program->HEAD - 1) 
                 == 'R'
                 && peek(program->source, program->HEAD)
                 == 'N'
                 && peek(program->source, program->HEAD + 1) 
                 == 'E' 
                 && peek(program->source, program->HEAD + 2) 
                 == 'G')
            {
                token_n->token   = "BRNEG";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_BRNEG;
                program->HEAD    += 4;
            }      //B
            else if(peek(program->source, program->HEAD - 1) 
                 == 'R')
            {
                token_n->token   = "BR";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_BR;
                program->HEAD    += 1;
            }
        break;
        case 'R':
            if(peek(program->source, program->HEAD - 1) 
                 == 'E'
                 && peek(program->source, program->HEAD)
                 == 'A'
                 && peek(program->source, program->HEAD + 1) 
                 == 'D')
            {
                token_n->token   = "READ";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_READ;
                program->HEAD    += 3;
            }
            else if(peek(program->source, program->HEAD - 1) 
                 == 'E'
                 && peek(program->source, program->HEAD)
                 == 'T')
            {
                token_n->token   = "RET";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_RET;
                program->HEAD    += 2;
            }
        break;
        case 'C':
            if(peek(program->source, program->HEAD - 1) 
                 == 'A'
                 && peek(program->source, program->HEAD)
                 == 'L'
                 && peek(program->source, program->HEAD + 1) 
                 == 'L')
            {
                token_n->token   = "CALL";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_CALL;
                program->HEAD    += 3;
            }
            else if(peek(program->source, program->HEAD - 1) 
                 == 'O'
                 && peek(program->source, program->HEAD)
                 == 'P'
                 && peek(program->source, program->HEAD + 1) 
                 == 'Y')
            {
                token_n->token   = "COPY";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_COPY;
                program->HEAD    += 3;
            }
        break;
        case 'A':
            if(peek(program->source, program->HEAD - 1) 
                 == 'D'
                 && peek(program->source, program->HEAD)
                 == 'D')
            {
                token_n->token   = "ADD";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_ADD;
                program->HEAD    += 2;
            }
        break;
        case 'D':
            if(peek(program->source, program->HEAD - 1) 
                 == 'I'
                 && peek(program->source, program->HEAD)
                 == 'V'
                 && peek(program->source, program->HEAD + 1)
                 == 'I'
                 && peek(program->source, program->HEAD + 2)
                 == 'D'
                 && peek(program->source, program->HEAD + 3)
                 == 'E')
            {
                token_n->token   = "DIVIDE";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_DIVIDE;
                program->HEAD    += 5;
            }
        break;
        case 'M':
            if(peek(program->source, program->HEAD - 1) 
                 == 'U'
                 && peek(program->source, program->HEAD)
                 == 'L'
                 && peek(program->source, program->HEAD + 1)
                 == 'T')
            {
                token_n->token   = "MULT";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_MULT;
                program->HEAD    += 3;
            }
        break;
        case 'W':
            if(peek(program->source, program->HEAD - 1) 
                 == 'R'
                 && peek(program->source, program->HEAD)
                 == 'I'
                 && peek(program->source, program->HEAD + 1)
                 == 'T'
                 && peek(program->source, program->HEAD + 2)
                 == 'E')
            {
                token_n->token   = "WRITE";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_WRITE;
                program->HEAD    += 4;
            }
        break;
        case '-': //comentario de unica linha
            if(peek(program->source, program->HEAD - 1) 
                 == '-')
            {
                while(program->HEAD < program->program_size
                && program->source[program->HEAD] != '\n'
                && program->source[program->HEAD] != '\0'){ //se chegou ao fim da string...
                    program->HEAD++;
                }
                /*comentario em inicio ou meio do programa esta tranquilo
                se um comentario for a ultima coisa escrita no programa, ultimo token eh NULL*/
                if(program->HEAD < program->program_size){
                    return nextToken(program);
                }
            }
        break;
        case '*':
            if(peek(program->source, program->HEAD - 1) 
                 == '-')
            {
                while(program->HEAD < program->program_size){
                    if(program->source[program->HEAD] == '-'
                    && peek(program->source, program->HEAD) == '*'){
                            program->HEAD += 2;
                            break;
                        }
                    program->HEAD++;
                }
                /*comentario em inicio ou meio do programa esta tranquilo
                se um comentario for a ultima coisa escrita no programa, ultimo token eh NULL*/
                if(program->HEAD < program->program_size){
                    return nextToken(program);
                }
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
                token_n->defined = true;
                token_n->value   = -1;
                token_n->type    = TOK_SECTION;
                program->HEAD    += 6;
            }
        break;
        case '.':
            if ( peek(program->source, program->HEAD - 1) 
                 == 't'
                 && peek(program->source, program->HEAD)
                 == 'e'
                 && peek(program->source, program->HEAD + 1) 
                 == 'x' 
                 && peek(program->source, program->HEAD + 2) 
                 == 't'  )
            {
                token_n->token   = ".text";
                token_n->defined = true;
                token_n->value   = -1;
                token_n->type    = TOK_SECTION_NAME;
                program->HEAD    += 4;
            }else if ( peek(program->source, program->HEAD - 1) 
                 == 'd'
                 && peek(program->source, program->HEAD)
                 == 'a'
                 && peek(program->source, program->HEAD + 1) 
                 == 't' 
                 && peek(program->source, program->HEAD + 2) 
                 == 'a'  )
            {
                token_n->token   = ".data";
                token_n->defined = true;
                token_n->type    = TOK_SECTION_NAME;
                token_n->value   = -1;
                program->HEAD    += 4;
            }else if ( peek(program->source, program->HEAD - 1) 
                 == 'w'
                 && peek(program->source, program->HEAD)
                 == 'o'
                 && peek(program->source, program->HEAD + 1) 
                 == 'r' 
                 && peek(program->source, program->HEAD + 2) 
                 == 'd'  )
            {
                token_n->token   = ".word";
                token_n->defined = false;
                token_n->type    = TOK_WORD;
                token_n->value   = -1;
                program->HEAD    += 4;
            }
        break;
        default:{ //abre-fecha chaves para permitir declarar identifier
            bool identifier = false;
            program->HEAD -= 1;
            while ( program->HEAD < program->program_size && 
                 isalpha(program->source[program->HEAD]) )
            {
               identifier = true;
               temp = ( char * ) realloc ( temp, c + 2 );
               assert( temp != NULL );
               temp[c] = program->source[program->HEAD]; 
               c++;
               program->HEAD++;
               temp[c] = '\0';
            }

            if ( identifier ) 
            {   
                if ( program->source[program->HEAD] == ':' ) 
                {
                    temp = ( char * ) realloc ( temp, c + 2 );
                    temp[c++] = ':';
                    temp[c] = '\0';
                    token_n->token   = temp;
                    token_n->defined = true;
                    token_n->value   = -1;
                    token_n->type    = TOK_LABEL;
                    program->HEAD++;
                }else 
                {
                    token_n->token   = temp;
                    token_n->defined = false;
                    token_n->value   = -1;
                    token_n->type    = TOK_IDENTIFIER;
                }
            }
        }
        break;
    }

    return *token_n;
}

token_t *getNextToken( program_t *program )
{
    if ( program->token_idx < program->n_tokens ) 
    { 
        return &program->tokens[program->token_idx++];
    }else 
    {
        return NULL;
    }
}

void searchAndUpdate( symbol_table_t *table ) 
{
}

void resolveIdentifiers( program_t *program ) 
{
    token_t *tok =
        getNextToken( program );

    int pc = 0;
    while ( program->token_idx < program->n_tokens ) 
    {
        switch( tok->type ) 
        {
            case TOK_WORD:

            break;
            case TOK_ASCIIZ:

            break;
            case TOK_LABEL:

            break;
            case TOK_IDENTIFIER:

            break;
            case TOK_STORE:
            case TOK_LOAD:
            case TOK_ADD:
            case TOK_SUB:
                pc+=2;
            break;
            case TOK_COPY:
                pc+=3;
            break;
            default:
            break;
        }
        tok = getNextToken(program);
    }
}

void parse( program_t *program ) 
{
    assert( program != NULL && program->tokens != NULL );
    int n_secs = 0; 

    Vector *dot_text = malloc(sizeof(Vector));
    initVector( dot_text, 10 ); 

    Vector *dot_data = malloc(sizeof(Vector));
    initVector( dot_data, 3 ); 

    Vector *dot_rodata = malloc(sizeof(Vector));
    initVector( dot_rodata, 2 ); 

    program->sections = 
        ( sections_t * ) malloc( sizeof( sections_t ) );

    program->sections->dot_data   = dot_data;
    program->sections->dot_text   = dot_text;
    program->sections->dot_rodata = dot_rodata;

    symbol_table_t *table = 
        malloc ( sizeof ( symbol_table_t ) );
    program->table = table;

    resolveIdentifiers(program);
    program->token_idx = 0;

    token_t *tok =
        getNextToken( program );

    while ( program->token_idx < program->n_tokens ) 
    {
        switch ( tok->type ) 
        {
            case TOK_LOAD:
                if( parseLoad(program, tok) <= -1 )
                    return;
            break;
            case TOK_STORE:
                if( parseStore(program, tok) <= -1 )
                    return;
            break;

            default:
            break;


        }
        tok = getNextToken(program);
    }

}

