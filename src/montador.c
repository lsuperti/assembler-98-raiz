
#include "montador.h"

#define WHITESPACES " \n\t\r"

// Função para visualizar os tokens 
// gerados por tokenize.
void printTokens( program_t *program ) 
{
    assert(program->tokens != NULL);
    fprintf( stdout, "{ " );
    fflush(stdout);
    for ( int i=0; i < program->n_tokens; i++ )
    {
        if ( program->tokens[i].token != NULL )
        {
            fprintf( stdout, "%s, ", program->tokens[i].token );
            fflush(stdout);
        }
        if ( ( i + 1 ) % 3 == 0 ) { fprintf( stdout, "\n" ); }
    }
    fprintf( stdout, " }" );
    fflush(stdout);
}

// Coloca as seções no arquivo ( objeto ) binario 
// por agora, como o formato já estabelecido em 
// program.elf98
void generateOutput( program_t *program, FILE *output )
{
   fprintf( output,  magic );  
   fprintf( output, "\nsection .text\n" ); 
   for (int i=0; i < program->sections->dot_text->used; i++ )
   {
       fprintf( output, "%d ", program->sections->dot_text->array[i] );
       if ( ( i + 1 ) % 8 == 0 ) { fprintf( output, "\n" ); }
   }
   fprintf( output, "\nsection .data\n");
   for (int i=0; i < program->sections->dot_data->used; i++ )
   {
       fprintf( output, "%d ", program->sections->dot_data->array[i] );
       if ( ( i + 1 ) % 8 == 0 ) { fprintf( output, "\n" ); }
   }
   fprintf( output, "\nsection .rodata\n");
   for (int i=0; i < program->sections->dot_rodata->used; i++ )
   {
       fprintf( output, "%d ", program->sections->dot_rodata->array[i] );
       if ( ( i + 1 ) % 8 == 0 ) { fprintf( output, "\n" ); }
   }
}

// Função que recebe dois caminhos de arquivo 
// e gera um binario.
program_t *assembleProgram( char *file_path, char *output_path ) 
{
   FILE *i = fopen( file_path, "rb" ); 
   program_t *program = createProgram(i);
   fclose(i);
   tokenize(program);
   parse(program);
   FILE *o = fopen( output_path, "w" );
   generateOutput( program, o );
   fclose(o);
   return program;
}

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

    buffer[lSize] = '\0';
    program->source       = buffer;
    program->HEAD         = 0;             
    program->tokens       = NULL;
    program->token_idx    = 0;
    program->n_tokens     = 0;
    program->sections     = NULL;
    program->table        = NULL;
    program->program_size = lSize;
    program->globals      = NULL;
    program->c_row        = 1;
    program->c_col        = 1;
    program->n_globals    = 0;
    program->externs      = NULL;
    program->n_externs    = 0;
    program->macros       = NULL;

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

// Faz a tokenização do programa já
// criado por createProgram até
// encontrar um token TOK_EOF.
void tokenize( program_t *program )
{ 
    int capacity = 10;
    token_t *tokens = (token_t *) malloc(sizeof(token_t) * capacity);
    assert(tokens != NULL);

    token_t tok;
    int idx = 0;

    do { 
        tok = nextToken(program);
        
        if (idx >= capacity) {
            capacity *= 2;
            tokens = (token_t *) realloc(tokens, sizeof(token_t) * capacity);
            assert(tokens != NULL);
        }

        tokens[idx++] = tok;
        // printf("%s: %d:%d\n", tok.token, tok.line, tok.column);
    } while (tok.type != TOK_EOF);

    if (idx > 0) {
        tokens = (token_t *) realloc(tokens, sizeof(token_t) * idx);
        assert(tokens != NULL);

        program->tokens = tokens;
        program->n_tokens = idx;
    } else {
        free(tokens);
        program->tokens = NULL;
        program->n_tokens = 0;
    }
}

/* 
 * Peeks for next character also checks if 
 * that is possible, returns empty if no next character since the 
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

token_t digit(program_t *program)
{
    token_t token_n;
    token_n.type = TOK_UNKNOWN;

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
       program->c_col++;
       program->HEAD++;
       temp[c] = '\0';
    }
    
    if ( digit ) 
    {
        token_n.token   = temp;
        token_n.defined = true;
        token_n.value   = strtol(temp, NULL, 10);
        token_n.type    = TOK_LITERAL;
        token_n.offset  = program->HEAD - strlen(temp);
        return token_n;
    }

    token_n.line    = program->c_row;
    return token_n;
}

token_t hex_digit(program_t *program)
{
    token_t token_n;
    token_n.type = TOK_UNKNOWN;

    char *temp = ( char * ) malloc ( 3 );
    assert ( temp != NULL );
    temp[0] = '\0';
    int c = 2;
    bool h_digit = false;

    if (    program->HEAD < program->program_size
         && program->source[program->HEAD] == '0' 
         && peek(program->source, program->HEAD) == 'x'
         && isxdigit( peek(program->source, program->HEAD + 1) ) )
    {
       h_digit = true;
       strcpy( temp, "0x" );
       program->c_col+=2;
       program->HEAD+=2;
    }
    
    while ( program->HEAD < program->program_size &&
            h_digit == true &&
            isxdigit(program->source[program->HEAD]) ) 
    {
        temp = ( char * ) realloc ( temp, c + 2 );
        assert( temp != NULL );
        temp[c] = program->source[program->HEAD]; 
        c++;
        program->c_col++;
        program->HEAD++;
        temp[c] = '\0';
    } 

    if ( h_digit == true ) 
    {
        token_n.token   = temp;
        token_n.defined = true;
        token_n.value   = strtol(temp, NULL, 16);
        token_n.type    = TOK_LITERAL_HEX;
        token_n.offset  = program->HEAD - strlen(temp);
        return token_n;
    }

    token_n.line    = program->c_row;
    return token_n;
}
/* Retorna um token NULL se não existem mais tokens */
/* Exemplo tokens válidos : 
 *      LOAD 
 *      203 
 *      RET
 *
 * Sempre retorna apenas um token. 
 * ( O próximo token no arquivo ) 
 *  Usa o index HEAD que começa em 0.
*/
token_t nextToken( program_t *program ) { 

    token_t *token_n = (token_t *) malloc ( sizeof ( token_t ) );
    assert( token_n != NULL );

    token_n->token   = "UNKNOWN";
    token_n->defined = true;
    token_n->value   = -1;
    token_n->type    = TOK_UNKNOWN;

    bool LOCAL_LABEL = false;

    // Pula espaços em branco e conta a numeração da linha
    while ( program->HEAD < program->program_size )
    {
        switch (program->source[program->HEAD])
        {
            case '\r':
                token_n->token   = "\\n";
                token_n->defined = true;
                token_n->value   = -1;
                token_n->type    = TOK_NEWLINE;
                
                // CRLF
                if ( peek(program->source, program->HEAD) == '\n' )
                {
                    program->HEAD += 2;
                    program->c_row++;
                    program->c_col = 1;

                    return *token_n;
                // CR
                } else
                {
                    program->HEAD++;
                    program->c_col = 1;

                    return *token_n;
                }
                break;
            // LF
            case '\n':
                program->HEAD++;
                program->c_row++;
                program->c_col = 1;

                token_n->token   = "\\n";
                token_n->defined = true;
                token_n->value   = -1;
                token_n->type    = TOK_NEWLINE;

                return *token_n;

                break;
            case '\t':
            case ' ':
                program->HEAD++;
                program->c_col++;
                break;
            default:
                goto break_whitespace_loop;
        }
        continue;
        
        break_whitespace_loop:
        break;
    }
    
    int tok_col = program->c_col;

    if ( program->HEAD >= program->program_size )  
    {
        token_n->token   = "EOF";
        token_n->type    = TOK_EOF;
        token_n->value   = -1;
        token_n->defined = true;
        token_n->offset  = program->HEAD;
        token_n->line    = program->c_row;
        token_n->column  = tok_col;
        return *token_n;
    }


    *token_n = hex_digit(program);
    if ( token_n->type != TOK_UNKNOWN ) {
        token_n->column = tok_col;
        return *token_n;
    }
    *token_n = digit(program);
    if ( token_n->type != TOK_UNKNOWN )  {
        token_n->column = tok_col;
        return *token_n;
    }

    char *temp;
    token_n->offset = program->HEAD;
    bool reserved = false;

    program->c_col++;
    switch ( program->source[program->HEAD++] ) {
        case '&':
           token_n->token   = "&";
           token_n->defined = true;
           token_n->value   = -1;
           token_n->type    = TOK_ADDRESSING;
           reserved = true;
           break;
        case '#':
           token_n->token   = "#"; 
           token_n->defined = true;
           token_n->value   = -1;
           token_n->type    = TOK_ADDRESSING;
           reserved = true;
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
                program->c_col   += 3;
                reserved = true;
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
                program->c_col   += 4;
                reserved = true;
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
                program->c_col   += 3;
                reserved = true;
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
                program->c_col   += 2;
                reserved = true;
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
                program->c_col   += 4;
                reserved = true;
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
                program->c_col   += 5;
                reserved = true;
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
                program->c_col   += 4;
                reserved = true;
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
                program->c_col   += 4;
                reserved = true;
            }      //B
            else if(peek(program->source, program->HEAD - 1) 
                 == 'R')
            {
                token_n->token   = "BR";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_BR;
                program->HEAD    += 1;
                program->c_col   += 1;
                reserved = true;
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
                program->c_col   += 3;
                reserved = true;
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
                program->c_col   += 2;
                reserved = true;
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
                program->c_col   += 3;
                reserved = true;
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
                program->c_col   += 3;
                reserved = true;
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
                program->c_col   += 2;
                reserved = true;
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
                program->c_col   += 5;
                reserved = true;
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
                program->c_col   += 3;
                reserved = true;
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
                program->c_col   += 4;
                reserved = true;
            }
        break;
        case '-': 
            if(peek(program->source, program->HEAD - 1) 
                 == '-')
            {
                program->HEAD++;
                while(program->HEAD < program->program_size
                && program->source[program->HEAD] != '\n' )
                {
                    program->HEAD++;
                    program->c_col++;
                }

                free(token_n);
                return nextToken(program);
                
            }
        break;
        case '*':
            if(peek(program->source, program->HEAD - 1) 
                 == '-')
            {
                program->HEAD++;
                while(program->HEAD < program->program_size){

                    if(program->source[program->HEAD] == '-'
                    && peek(program->source, program->HEAD) == '*'){
                        program->HEAD  += 2;
                        program->c_col += 2;
                        break;
                    }

                    program->HEAD++;
                    program->c_col++;
                }

                free(token_n);
                return nextToken(program);
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
                program->c_col   += 6;
                reserved = true;
            }
        break;
        case '%':
            if ( peek(program->source, program->HEAD - 1) 
                 == 'm'
                 && peek(program->source, program->HEAD)
                 == 'a'
                 && peek(program->source, program->HEAD + 1) 
                 == 'c' 
                 && peek(program->source, program->HEAD + 2) 
                 == 'r' 
                 && peek(program->source, program->HEAD + 3) 
                 == 'o' )
            {
                token_n->token   = "%macro";
                token_n->defined = true;
                token_n->value   = -1;
                token_n->type    = TOK_MACRO_START;
                program->HEAD    += 5;
                program->c_col   += 5;
                reserved = true;
            }else if ( peek(program->source, program->HEAD - 1) 
                 == 'e'
                 && peek(program->source, program->HEAD)
                 == 'n'
                 && peek(program->source, program->HEAD + 1) 
                 == 'd' 
                 && peek(program->source, program->HEAD + 2) 
                 == 'm' 
                 && peek(program->source, program->HEAD + 3) 
                 == 'a' 
                 && peek(program->source, program->HEAD + 4) 
                 == 'c'
                 && peek(program->source, program->HEAD + 5) 
                 == 'r' 
                 && peek(program->source, program->HEAD + 6) 
                 == 'o' )
            {
                token_n->token   = "%endmacro";
                token_n->defined = true;
                token_n->value   = -1;
                token_n->type    = TOK_MACRO_END;
                program->HEAD    += 8;
                program->c_col   += 8;
                reserved = true;
            }else if ( peek(program->source, program->HEAD - 1) == '%' )
            {
                LOCAL_LABEL = true;
                program->HEAD+=2;
                goto check_identifier;
            }
        break;
        case 'g':
            if ( peek(program->source, program->HEAD - 1) 
                 == 'l'
                 && peek(program->source, program->HEAD)
                 == 'o'
                 && peek(program->source, program->HEAD + 1) 
                 == 'b' 
                 && peek(program->source, program->HEAD + 2) 
                 == 'a' 
                 && peek(program->source, program->HEAD + 3) 
                 == 'l' )
            {
                token_n->token   = "global";
                token_n->defined = true;
                token_n->value   = -1;
                token_n->type    = TOK_GLOBAL;
                program->HEAD    += 5;
                program->c_col   += 5;
                reserved = true;
            }
        break;
        case 'e':
            if ( peek(program->source, program->HEAD - 1) 
                 == 'x'
                 && peek(program->source, program->HEAD)
                 == 't'
                 && peek(program->source, program->HEAD + 1) 
                 == 'e' 
                 && peek(program->source, program->HEAD + 2) 
                 == 'r' 
                 && peek(program->source, program->HEAD + 3) 
                 == 'n' )
            {                       
                token_n->token   = "extern";
                token_n->defined = true;
                token_n->value   = -1;
                token_n->type    = TOK_GLOBAL;
                program->HEAD    += 5;
                program->c_col   += 5;
                reserved = true;
            }
        break;
        case 'P':
            if(peek(program->source, program->HEAD - 1) 
                 == 'U'
                 && peek(program->source, program->HEAD)
                 == 'T' )
            {
                token_n->token   = "PUT";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_PUT;
                program->HEAD    += 2;
                program->c_col   += 2;
                reserved = true;
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
                program->c_col   += 4;
                reserved = true;
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
                program->c_col   += 4;
                reserved = true;
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
                token_n->defined = true;
                token_n->type    = TOK_WORD;
                token_n->value   = -1;
                program->HEAD    += 4;
                program->c_col   += 4;
                reserved = true;
            }else if ( peek(program->source, program->HEAD - 1) 
                 == 's'
                 && peek(program->source, program->HEAD)
                 == 'p'
                 && peek(program->source, program->HEAD + 1) 
                 == 'a' 
                 && peek(program->source, program->HEAD + 2) 
                 == 'c' 
                 && peek(program->source, program->HEAD + 3) 
                 == 'e'  )
            {
                token_n->token   = ".space";
                token_n->defined = true;
                token_n->type    = TOK_SPACE;
                token_n->value   = -1;
                program->HEAD    += 5;
                program->c_col   += 5;
                reserved = true;
            }
        break;
        default:{
    check_identifier:      
            token_n->line    = program->c_row;
            token_n->column  = tok_col;
            temp = ( char * ) malloc ( 3 );
            assert ( temp != NULL );
            int c = 1;
            bool identifier = false;

            program->HEAD--;
            program->c_col--;
            if ( program->HEAD < program->program_size && 
                 isalpha(program->source[program->HEAD]) )
            {
               identifier = true;
               temp[0] = program->source[program->HEAD++];
               program->c_col++;
               temp[1] = '\0';
            }

            while (   identifier && 
                     isalnum(program->source[program->HEAD])
                     || program->source[program->HEAD] == '_' ) 
            {
                temp = ( char * ) realloc ( temp, c + 2 );
                assert( temp != NULL );
                temp[c] = program->source[program->HEAD]; 
                c++;
                program->HEAD++;
                program->c_col++;
                temp[c] = '\0';
            }

            if ( identifier ) 
            {   
                if ( program->source[program->HEAD] == ':')
                {
                    temp = ( char * ) realloc ( temp, c + 2 );
                    temp[c++] = ':';
                    temp[c] = '\0';
                    token_n->token   = temp;
                    token_n->defined = true;
                    token_n->value   = -1;
                    token_n->type    = TOK_LABEL;
                    program->HEAD++;
                    program->c_col++;
                    if ( LOCAL_LABEL )
                    {
                        char *prefix = malloc(3 + strlen(temp) );
                        strcpy( prefix, "%%" ); 
                        strcat( prefix, temp );
                        token_n->token = prefix;
                        free(temp);
                        token_n->type  = TOK_LOCAL_LABEL;
                    }
                }else 
                {
                    token_n->token   = temp;
                    token_n->defined = false;
                    token_n->value   = -1;
                    token_n->type    = TOK_IDENTIFIER;
                    if ( LOCAL_LABEL ) 
                    {
                        char *prefix = malloc(3 + strlen(temp) );
                        strcpy( prefix, "%%" ); 
                        strcat( prefix, temp );
                        token_n->token = prefix;
                        free(temp);
                        token_n->type = TOK_LOCAL_IDENTIFIER;
                    }
                }
            }
            program->HEAD++;
            program->c_col++;
        }
        return *token_n;
        break;
    }

    if ( !reserved ) 
        goto check_identifier;

    token_n->line    = program->c_row;
    token_n->column  = tok_col;
    return *token_n;
}

/* 
 * Pega o proximo token do programa depois
 * de já feita a tokenização ( Mutável muda HEAD )
 * retorna NULL se não existe.
*/
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

// Pesquisa por um token 
// retorna NULL se não existe.
token_t *searchIdent( symbol_table_t *table, char *token ) 
{
    return NULL;
}

// Adiciona um token na tabela de simbolos dinâmicamente
void appendTok( symbol_table_t *table, token_t *tok ) 
{
    assert( table != NULL && tok != NULL );
    if ( table->tokens == NULL ) 
    {
        table->tokens = 
            malloc( sizeof( token_t ) * ( table->num_s + 2 ) );
    }else 
    {
        table->tokens = 
            realloc( table->tokens,
                    sizeof( token_t ) * ( table->num_s + 2 ) );
    }

    table->tokens[ table->num_s++ ] = *tok;
}

// Retorna o token anterior ao atual, não mutável.
token_t *peek_previous_token( program_t *program )
{
    assert( program != NULL );
    if ( program->token_idx - 1 > 0 ) 
    {
        return &program->tokens[ program->token_idx - 1 ];
    }else 
    {
        return NULL;
    }
}

// Retorna o token após o atual, não mutável.
token_t *peek_token( program_t *program )
{
    assert( program != NULL );
    if ( program->token_idx < program->n_tokens ) 
    {
        return &program->tokens[ program->token_idx ];
    }else 
    {
        return NULL;
    }
}

// Função para retirar o ultimo character de uma 
// string. 
void cut_last ( char *str ) 
{
    if ( 0 < strlen( str ) ) 
    {
        str[ strlen( str ) - 1 ] = '\0'; 
    }else
    {
        return;
    }
}

// Função que resolve os identificadores
// os colocando depois de definidos na tabela
// de símbolos.
int resolveIdentifiers( program_t *program ) 
{
    token_t *tok =
        getNextToken( program );
    token_t *u_tok, *n_tok;
    
    int pc = 0;
    int dr_idx = 0;
    while ( program->token_idx < program->n_tokens ) 
    {
        switch( tok->type ) 
        {
            case TOK_LABEL: {
                char *str;
                str = malloc ( strlen ( tok->token ) + 1 );
                strcpy (str, tok->token);
                cut_last(str);
                for ( int i=0; i < program->table->num_s; i++ )
                {
                    if (
                    strcmp ( program->table->tokens[i].token, str ) == 0 ) 
                    {
                        current_parser_error = 
                        realloc ( current_parser_error, 100 + strlen(tok->token) );
                        snprintf( current_parser_error, 100,
                        "Multiple definitions of identifier { %s }"
                        " first defined at line { %d }",
                        tok->token, tok->line );
                        gtk_text_buffer_set_text(cpe, current_parser_error, -1);
                        return -1;
                    }
                }
                n_tok = malloc( sizeof( token_t ) );
                u_tok = peek_token( program );
                if ( u_tok->type == TOK_WORD ||
                     u_tok->type == TOK_SPACE ) 
                {
                    n_tok->value = data_reg + (dr_idx++);

                    n_tok->token = malloc( strlen( tok->token ) );
                    n_tok->token = strcpy( n_tok->token, tok->token);
                    cut_last( n_tok->token );

                    n_tok->defined = true;
                    n_tok->type = TOK_IDENTIFIER;
                    appendTok( program->table, n_tok );
                }else if ( u_tok->type == TOK_ASCIIZ ) 
                {
                }else
                {
                    n_tok->value = pc + TEXT_SEGMENT_START;
                    n_tok->token = malloc( strlen( tok->token ) );
                    n_tok->token = strcpy( n_tok->token, tok->token );
                    cut_last( n_tok->token );
                    n_tok->defined = true;
                    n_tok->type = TOK_IDENTIFIER;
                    appendTok( program->table, n_tok );
                }
            }
            break;
            // Incrementa um pc imaginario para 
            // colocar o valor dos labels.
            case TOK_STORE:
            case TOK_LOAD:
            case TOK_ADD:
            case TOK_SUB:
            case TOK_BRNEG:
            case TOK_BRZERO:
            case TOK_BRPOS:
            case TOK_CALL:
            case TOK_DIVIDE:
            case TOK_MULT:
            case TOK_READ:
            case TOK_WRITE:
            case TOK_PUT:
            case TOK_STOP:
                pc+=2;
            break;
            case TOK_RET:
                pc++;
            break;
            case TOK_COPY:
                pc+=3;
            break;
            default:
            break;
        }
        tok = getNextToken(program);
    }
    return EXIT_SUCCESS;

}

// Função que faz o parsing, atualiza 
// os vetores ( dot_data, dot_text, dot_rodata )
// que serão colocados no arquivo de saída.
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

    GtkTextView *console_errors = 
        GTK_TEXT_VIEW(gtk_builder_get_object(p_builder, "consoleErros" ));

    GtkTextBuffer *console_ebuffer =
        gtk_text_view_get_buffer( console_errors );

    cpe = console_ebuffer;

    program->sections->dot_data   = dot_data;
    program->sections->dot_text   = dot_text;
    program->sections->dot_rodata = dot_rodata;

    symbol_table_t *table = 
        malloc ( sizeof ( symbol_table_t ) );
    program->table = table;
    program->table->num_s  = 0;
    program->table->tokens = NULL;

    int res = resolveIdentifiers(program);
    if ( res <= -1 ) 
        return;

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
            case TOK_WORD:
                if( parseWord(program, tok) <= -1 )
                    return;
            break;
            case TOK_SPACE:
               insert( program->sections->dot_data, 0 ); 
            break;
            case TOK_STOP:
               insert( program->sections->dot_text, STOP ); 
            break;
            case TOK_BR:
                if( parseBR(program, tok) <= -1 )
                    return;
            break;
            case TOK_BRNEG:
                if( parseBRNEG(program, tok) <= -1 )
                    return;
            break;
            case TOK_BRZERO:
                if( parseBRZERO(program, tok) <= -1 )
                    return;
            break;
            case TOK_BRPOS:
                if( parseBRPOS(program, tok) <= -1 )
                    return;
            break;
            case TOK_ADD:
                if( parseADD(program, tok) <= -1 )
                    return;
            break;
            case TOK_SUB:
                if( parseSUB(program, tok) <= -1 )
                    return;
            break;
            case TOK_READ:
                if( parseRead(program, tok) <= -1 )
                    return;
            break;
            case TOK_PUT:
                if( parsePUT(program, tok) <= -1 )
                    return;
            break;

            default:
            break;

        }
        tok = getNextToken(program);
    }

}

