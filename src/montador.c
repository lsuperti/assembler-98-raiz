
#include "montador.h"

#define WHITESPACES " \n\t\r"

typedef struct {
    GtkTextBuffer *buffer;
    program_t     *program;
    gulong        *id;
} update_pass;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond   = PTHREAD_COND_INITIALIZER;
gboolean update_done  = FALSE;
guint update_timeout_id = 0;

gboolean update_text( gpointer data )
{
    update_pass *p = data;
    GtkTextIter cursor_iter;

    g_signal_handler_block(p->buffer, *p->id);

    GtkTextMark *insert_mark = gtk_text_buffer_get_insert(p->buffer);
    gtk_text_buffer_get_iter_at_mark(p->buffer, &cursor_iter, insert_mark);

    for( int i=0; i < p->program->n_tokens - 1; i++ )
    {
        int type = p->program->tokens[i].type;
        const char *color = tok_colors[type];

        if ( color != NULL && type != TOK_UNKNOWN ) 
        {
            // Offset coloring is broken currently
            // idk why, a fix is to subtract one from the first argument
            // program->tokens[i].offset - 1
            // but that is not wanted.
            if ( p->program->tokens[i].offset - 1 > 0 )
                colorize_token( p->buffer, p->program->tokens[i].offset - 1, 
                        p->program->tokens[i].offset + strlen(p->program->tokens[i].token),
                        color );
            else 
                colorize_token( p->buffer, p->program->tokens[i].offset, 
                        p->program->tokens[i].offset + strlen(p->program->tokens[i].token),
                        color );
        }
    }

    gtk_text_buffer_place_cursor(p->buffer, &cursor_iter);
    // Unlock signal after updating syntax coloring
    // DO NOT REMOVE !! otherwise 100% CPU usage.
    g_signal_handler_unblock(p->buffer, *p->id);

    freeProgram(p->program);
    free(p);
    return FALSE;
}

void            on_buffer_changed( GtkWidget *w, gpointer data )
{

    gchar *source;
    GtkTextIter start, end;
    GtkTextBuffer *b = GTK_TEXT_BUFFER(w);
    gtk_text_buffer_get_bounds(b, &start, &end);
    source = gtk_text_buffer_get_text(b, &start, &end, FALSE);
    program_t *program = malloc(sizeof(program_t));

    program->source       = source;
    program->HEAD         = 0;             
    program->tokens       = NULL;
    program->token_idx    = 0;
    program->n_tokens     = 0;
    program->sections     = NULL;
    program->table        = NULL;
    program->program_size = strlen(source);
    program->globals      = NULL;
    program->c_row        = 1;
    program->c_col        = 1;
    program->n_globals    = 0;
    program->externs      = NULL;
    program->n_externs    = 0;
    program->macros       = NULL;
    program->n_macros     = 0;
    program->cur_macro_params = NULL;
    tokenize(program);

    update_pass *p = malloc(sizeof(update_pass));
    p->buffer  = b;
    p->program = program;
    p->id      = data;

    g_idle_add(update_text, p);
}

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

   fprintf( output, "\nglobal\n");
   token_t *el, *tmp;
   HASH_ITER(hh, program->globals, el, tmp )
   {
     if ( el->data_l == true )
     {
         fprintf( output, "\t%s& %d\n", el->token,
                 el->value );
     }
     else
     {
         fprintf( output, "\t%s %d\n", el->token,
                 el->value );
     }
     fflush(stdout);
   }

   fprintf( output, "\nextern\n" );
   HASH_ITER(hh, program->externs, el, tmp )
   {
     if ( el->pos.used > 0 )
         fprintf( output, "\t%s ", el->token );
     for ( int i=0; i < el->pos.used; i++ ) 
     {
         if ( el->pos.array != NULL )
            fprintf( output, "%d ", el->pos.array[i] );
     }
     if ( el->pos.used > 0 )
         fprintf( output, "\n" );
   }

   fprintf( output, "\nlocal_text_labels\n" );
   HASH_ITER(hh, program->table->tokens, el, tmp )
   {
     token_t *tmp2; 
     char    *n = el->token;
     HASH_FIND_STR(program->externs, n, tmp2);
     el->pos = find_all_identifier_pos( program, el );

     if ( tmp2 == NULL && el->data_l == false )
     {
         if ( el->pos.used > 0 )
             fprintf( output, "\t%s ", el->token );
         for ( int i=0; i < el->pos.used; i++ ) 
         {
             if ( el->pos.array != NULL )
                fprintf( output, "%d ", el->pos.array[i] );
         }
         if ( el->pos.used > 0 )
             fprintf( output, "\n" );
     }

     fflush(stdout);
   }
    
   fprintf( output, "\nlocal_data_labels\n" );
   HASH_ITER(hh, program->table->tokens, el, tmp )
   {
     token_t *tmp2; 
     char    *n = el->token;
     HASH_FIND_STR(program->externs, n, tmp2);
     el->pos = find_all_identifier_pos( program, el );

     if ( tmp2 == NULL && el->data_l == true )
     {
         if ( el->pos.used > 0 )
             fprintf( output, "\t%s ", el->token );
         for ( int i=0; i < el->pos.used; i++ ) 
         {
             if ( el->pos.array != NULL )
                fprintf( output, "%d ", el->pos.array[i] );
         }
         if ( el->pos.used > 0 )
             fprintf( output, "\n" );
     }

     fflush(stdout);
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
   resetIdentifiers_Macros(program);
   fclose(o);
   return program;
}

/**
 * Esta funcao recebe arquivo contendo codigo fonte do pc hipotetico
 * e cria um programa pronto para montagem.
 * 
 * @param file arquivo contendo codigo fonte do pc hipotetico
 * @return program (struct program_t) pronto para traducao
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
    program->source           = buffer;
    program->HEAD             = 0;             
    program->tokens           = NULL;
    program->token_idx        = 0;
    program->n_tokens         = 0;
    program->sections         = NULL;
    program->table            = NULL;
    program->program_size     = lSize;
    program->globals          = NULL;
    program->c_row            = 1;
    program->c_col            = 1;
    program->n_globals        = 0;
    program->externs          = NULL;
    program->n_externs        = 0;
    program->macros           = NULL;
    program->n_macros         = 0;
    program->cur_macro_params = NULL;

    return program;
}

void freeToken( token_t *t )
{
    if ( t != NULL )
    {
        if ( t->token != NULL )
        {
            free(t->token);
            t->token = NULL;
        }
        if ( t->formal != NULL )
        {
            free(t->formal);
            t->formal = NULL;
        }

        t->type = t->defined 
        = t->value = t->offset 
        = t->line = t->column
        = t->data_l = 0;

        free(t);
        t = NULL;
    }
}

/*
 * Libera a memoria da struct program_t.
*/
void freeProgram( program_t *program )
{
   if ( program != NULL ) {
        resetIdentifiers_Macros(program);
        if ( program->source != NULL )
        {
            free(program->source);
            program->source = NULL;
        }
        if ( program->sections != NULL )
        {
            freeVector(program->sections->dot_text);
            freeVector(program->sections->dot_data);
            freeVector(program->sections->dot_rodata);
            free(program->sections);
            program->sections = NULL;
        }

        program->n_tokens = program->token_idx
        = program->n_symbols = program->HEAD =
        program->program_size = program->c_row
        = program->c_col = 0;

        free(program);
        program = NULL;
   }
}

/**
 * Esta funcao faz a separacao em tokens do codigo fonte que program->source possui.
 * Os tokens sao inseridos no vetor program->tokens e o numero total de tokens em
 * program->n_tokens.
 * A tokenizacao ocorre ate encontrar um token do tipo end of file (TOK_EOF).
 * 
 * @param program struct program_t contendo informacoes sobre o programa a ser traduzido.
 */
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

        tokens[idx++] = tok; //armazena token no vetor e incrementa o cabecote
        // printf("%s: %d:%d\n", tok.token, tok.line, tok.column);
    } while (tok.type != TOK_EOF); // executa ate encontrar token do tipo EOF

    if (idx > 0) { //realoca vetor para tamanho total de tokens
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
/**
 * Esta funcao percorre o texto program->source a partir do cabecote program->HEAD
 * ate encontrar um token. A funcao sempre retorna apenas um token ao realizar a
 * leitura dos proximos caracteres. program->HEAD inicializa em zero e e 
 * incrementado conforme o texto source e varrido.
 * Exemplo tokens validos: {LOAD, 203, RET, STOP}
 * 
 * @param program struct program_t contendo codigo fonte para ser analisado.
 * @return token identificado ou token NULL se nao existirem mais tokens.
*/

// TODO: o tokenizador pode ser melhorado
// por que nesse momento se uma instrução for acompanhada
// de uma nova linha e não um espaço não será reconhecida
// como instrução e sim como identificador. 
// a função isWhiteSpace faria o papel de reconhecer se há
// um espaço a frente, algo parecido já foi feito 
// é so tirar aquela parte do código e mudar um pouco a lógica.
bool isWhiteSpace() 
{
    return false;
}

token_t nextToken( program_t *program ) { 

    // inicializa token
    token_t *token_n = (token_t *) malloc ( sizeof ( token_t ) );
    assert( token_n != NULL );
    token_n->token   = "UNKNOWN";
    token_n->defined = true;
    token_n->value   = -1;
    token_n->type    = TOK_UNKNOWN;
    token_n->formal  = NULL;

    bool LOCAL_LABEL = false;

    // Pula espaços em branco e conta a numeração da linha
    // Removi tokenização de newlines, não tem necessidade 
    // por agora, mas deixei comentado.
    while ( program->HEAD < program->program_size )
    {
        switch (program->source[program->HEAD])
        {
            case '\r':
               // token_n->token   = "\\n";
               // token_n->defined = true;
               // token_n->value   = -1;
               // token_n->type    = TOK_NEWLINE;
                
                // CRLF
                if ( peek(program->source, program->HEAD) == '\n' )
                {
                    program->HEAD += 2;
                    program->c_row++;
                    program->c_col = 1;
                //    return *token_n;
                // CR
                } else
                {
                    program->HEAD++;
                    program->c_row++;
                    program->c_col = 1;
                //   return *token_n;
                }
                break;
            // LF
            case '\n':
                program->HEAD++;
                program->c_row++;
                program->c_col = 1;

              //  token_n->token   = "\\n";
              //  token_n->defined = true;
              //  token_n->value   = -1;
              //  token_n->type    = TOK_NEWLINE;

                //return *token_n;

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

    // ao chegar ao final do arquivo, gera token do tipo EOF
    if ( program->HEAD >= program->program_size )  
    {
        token_n->token   = "EOF";
        token_n->type    = TOK_EOF;
        token_n->value   = -1;
        token_n->defined = true;
        token_n->offset  = program->HEAD;
        token_n->line    = program->c_row + 1;
        token_n->column  = tok_col;
        token_n->formal  = NULL;
        return *token_n;
    }

    // tratamento dos valores liteirais decimais e hexadecimais
    *token_n = hex_digit(program);
    if ( token_n->type != TOK_UNKNOWN ) {
        token_n->column = tok_col;
        token_n->formal = NULL;
        return *token_n;
    }
    *token_n = digit(program);
    if ( token_n->type != TOK_UNKNOWN )  {
        token_n->column = tok_col;
        token_n->formal = NULL;
        return *token_n;
    }

    char *temp;
    token_n->offset = program->HEAD;
    bool reserved = false;

    // tratamento dos tokens de instrucoes e diretivas de maquina
    program->c_col++;
    switch ( program->source[program->HEAD++] ) {
        case '&': // tipo de enderecamento
           token_n->token   = "&";
           token_n->defined = true;
           token_n->value   = -1;
           token_n->type    = TOK_ADDRESSING;
           reserved = true;
           break;
        case '#': // tipo de enderecamento
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
                 == 'D' 
                 && peek(program->source, program->HEAD + 2) 
                 == ' ' )
            {
                token_n->token   = "LOAD";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_LOAD;
                program->HEAD    += 4;
                program->c_col   += 4;
                reserved = true;
            }
        break;
        case '|':
           token_n->token   = "|";
           token_n->defined = true;
           token_n->value   = -1;
           token_n->type    = TOK_PARAM_DELIMITER;
           reserved         = true;
        break;
        case 'S':
            if ( peek(program->source, program->HEAD - 1) 
                 == 'T'
                 && peek(program->source, program->HEAD)
                 == 'O'
                 && peek(program->source, program->HEAD + 1) 
                 == 'R' 
                 && peek(program->source, program->HEAD + 2) 
                 == 'E'
                 && peek(program->source, program->HEAD + 3) 
                 == ' ')
            {
                token_n->token   = "STORE";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_STORE;
                program->HEAD    += 5;
                program->c_col   += 5;
                reserved         = true;
            }      //S 
            else if( peek(program->source, program->HEAD - 1)
                 == 'T'
                 && peek(program->source, program->HEAD)
                 == 'O'
                 && peek(program->source, program->HEAD + 1) 
                 == 'P' )
            {
                token_n->token   = "STOP";
                token_n->defined = true;
                token_n->value   = 11;
                token_n->type    = TOK_STOP;
                program->HEAD    += 3;
                program->c_col   += 3;
                reserved         = true;
            }      //S
            else if(peek(program->source, program->HEAD - 1)
                 == 'U'
                 && peek(program->source, program->HEAD)
                 == 'B'
                 && peek(program->source, program->HEAD + 1)
                 == ' ')
            {
                token_n->token   = "SUB";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_SUB;
                program->HEAD    += 3;
                program->c_col   += 3;
                reserved         = true;
            }
            else if (peek(program->source, program->HEAD - 1) 
                 == 'T'
                 && peek(program->source, program->HEAD)
                 == 'A'
                 && peek(program->source, program->HEAD + 1) 
                 == 'C' 
                 && peek(program->source, program->HEAD + 2) 
                 == 'K'
                 && peek(program->source, program->HEAD + 3) 
                 == ' ')
            {
                token_n->token   = "STACK";
                token_n->defined = true;
                token_n->value   = -1;
                token_n->type    = TOK_STACK;
                program->HEAD    += 5;
                program->c_col   += 5;
                reserved         = true;
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
                 == 'O'
                 && peek(program->source, program->HEAD + 4) 
                 == ' ')
            {
                token_n->token   = "BRZERO";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_BRZERO;
                program->HEAD    += 6;
                program->c_col   += 6;
                reserved         = true;
            }      //B
            else if(peek(program->source, program->HEAD - 1) 
                 == 'R'
                 && peek(program->source, program->HEAD)
                 == 'P'
                 && peek(program->source, program->HEAD + 1) 
                 == 'O' 
                 && peek(program->source, program->HEAD + 2) 
                 == 'S'
                 && peek(program->source, program->HEAD + 3) 
                 == ' ')
            {
                token_n->token   = "BRPOS";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_BRPOS;
                program->HEAD    += 5;
                program->c_col   += 5;
                reserved         = true;
            }      //B
            else if(peek(program->source, program->HEAD - 1) 
                 == 'R'
                 && peek(program->source, program->HEAD)
                 == 'N'
                 && peek(program->source, program->HEAD + 1) 
                 == 'E' 
                 && peek(program->source, program->HEAD + 2) 
                 == 'G'
                 && peek(program->source, program->HEAD + 3) 
                 == ' ')
            {
                token_n->token   = "BRNEG";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_BRNEG;
                program->HEAD    += 5;
                program->c_col   += 5;
                reserved         = true;
            }      //B
            else if(peek(program->source, program->HEAD - 1) 
                 == 'R'
                 && peek(program->source, program->HEAD) 
                 == ' ')
            {
                token_n->token   = "BR";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_BR;
                program->HEAD    += 2;
                program->c_col   += 2;
                reserved         = true;
            }
        break;
        case 'R':
            if(peek(program->source, program->HEAD - 1) 
                 == 'E'
                 && peek(program->source, program->HEAD)
                 == 'A'
                 && peek(program->source, program->HEAD + 1) 
                 == 'D'
                 && peek(program->source, program->HEAD + 2) 
                 == ' ')
            {
                token_n->token   = "READ";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_READ;
                program->HEAD    += 4;
                program->c_col   += 4;
                reserved         = true;
            }
            else if(peek(program->source, program->HEAD - 1) 
                 == 'E'
                 && peek(program->source, program->HEAD)
                 == 'T' )
            {
                token_n->token   = "RET";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_RET;
                program->HEAD    += 2;
                program->c_col   += 2;
                reserved         = true;
            }
        break;
        case 'C':
            if(peek(program->source, program->HEAD - 1) 
                 == 'A'
                 && peek(program->source, program->HEAD)
                 == 'L'
                 && peek(program->source, program->HEAD + 1) 
                 == 'L'
                 && peek(program->source, program->HEAD + 2) 
                 == ' ')
            {
                token_n->token   = "CALL";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_CALL;
                program->HEAD    += 4;
                program->c_col   += 4;
                reserved         = true;
            }
            else if(peek(program->source, program->HEAD - 1) 
                 == 'O'
                 && peek(program->source, program->HEAD)
                 == 'P'
                 && peek(program->source, program->HEAD + 1) 
                 == 'Y'
                 && peek(program->source, program->HEAD + 2) 
                 == ' ')
            {
                token_n->token   = "COPY";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_COPY;
                program->HEAD    += 4;
                program->c_col   += 4;
                reserved         = true;
            }
        break;
        case 'A':
            if(peek(program->source, program->HEAD - 1) 
                 == 'D'
                 && peek(program->source, program->HEAD)
                 == 'D'
                 && peek(program->source, program->HEAD + 1)
                 == ' ')
            {
                token_n->token   = "ADD";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_ADD;
                program->HEAD    += 3;
                program->c_col   += 3;
                reserved         = true;
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
                 == 'E'
                 && peek(program->source, program->HEAD + 4)
                 == ' ')
            {
                token_n->token   = "DIVIDE";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_DIVIDE;
                program->HEAD    += 6;
                program->c_col   += 6;
                reserved         = true;
            }
        break;
        case 'M':
            if(peek(program->source, program->HEAD - 1) 
                 == 'U'
                 && peek(program->source, program->HEAD)
                 == 'L'
                 && peek(program->source, program->HEAD + 1)
                 == 'T'
                 && peek(program->source, program->HEAD + 2)
                 == ' ')
            {
                token_n->token   = "MULT";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_MULT;
                program->HEAD    += 4;
                program->c_col   += 4;
                reserved         = true;
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
                 == 'E'
                 && peek(program->source, program->HEAD + 3)
                 == ' ')
            {
                token_n->token   = "WRITE";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_WRITE;
                program->HEAD    += 5;
                program->c_col   += 5;
                reserved         = true;
            }
        break;
        case '-': // comentario de linha unica
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
        case '*': //comentario de multiplas linhas
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
                 == 'n' 
                 && peek(program->source, program->HEAD + 5) 
                 == ' ' )
            {
                token_n->token   = "section";
                token_n->defined = true;
                token_n->value   = -1;
                token_n->type    = TOK_SECTION;
                program->HEAD    += 7;
                program->c_col   += 7;
                reserved         = true;
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
                reserved         = true;
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
                reserved         = true;
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
                 == 'l'
                 && peek(program->source, program->HEAD + 4) 
                 == ' ' )
            {
                token_n->token   = "global";
                token_n->defined = true;
                token_n->value   = -1;
                token_n->type    = TOK_GLOBAL;
                program->HEAD    += 6;
                program->c_col   += 6;
                reserved         = true;
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
                 == 'n' 
                 && peek(program->source, program->HEAD + 4) 
                 == ' ' )
            {                       
                token_n->token   = "extern";
                token_n->defined = true;
                token_n->value   = -1;
                token_n->type    = TOK_EXTERN;
                program->HEAD    += 6;
                program->c_col   += 6;
                reserved         = true;
            }
        break;
        case 'P':
            if(peek(program->source, program->HEAD - 1) 
                 == 'U'
                 && peek(program->source, program->HEAD)
                 == 'T' 
                 && peek(program->source, program->HEAD + 1)
                 == ' ' )
            {
                token_n->token   = "PUT";
                token_n->defined = false;
                token_n->value   = -1;
                token_n->type    = TOK_PUT;
                program->HEAD    += 3;
                program->c_col   += 3;
                reserved         = true;
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
                 == 't' )
            {
                token_n->token   = ".text";
                token_n->defined = true;
                token_n->value   = -1;
                token_n->type    = TOK_SECTION_NAME;
                program->HEAD    += 4;
                program->c_col   += 4;
                reserved         = true;
            }else if ( peek(program->source, program->HEAD - 1) 
                 == 'd'
                 && peek(program->source, program->HEAD)
                 == 'a'
                 && peek(program->source, program->HEAD + 1) 
                 == 't' 
                 && peek(program->source, program->HEAD + 2) 
                 == 'a' )
            {
                token_n->token   = ".data";
                token_n->defined = true;
                token_n->type    = TOK_SECTION_NAME;
                token_n->value   = -1;
                program->HEAD    += 4;
                program->c_col   += 4;
                reserved         = true;
            }else if ( peek(program->source, program->HEAD - 1) 
                 == 'w'
                 && peek(program->source, program->HEAD)
                 == 'o'
                 && peek(program->source, program->HEAD + 1) 
                 == 'r' 
                 && peek(program->source, program->HEAD + 2) 
                 == 'd' )
            {
                token_n->token   = ".word";
                token_n->defined = true;
                token_n->type    = TOK_WORD;
                token_n->value   = -1;
                program->HEAD    += 4;
                program->c_col   += 4;
                reserved         = true;
            }else if ( peek(program->source, program->HEAD - 1) 
                 == 's'
                 && peek(program->source, program->HEAD)
                 == 'p'
                 && peek(program->source, program->HEAD + 1) 
                 == 'a' 
                 && peek(program->source, program->HEAD + 2) 
                 == 'c' 
                 && peek(program->source, program->HEAD + 3) 
                 == 'e' )
            {
                token_n->token   = ".space";
                token_n->defined = true;
                token_n->type    = TOK_SPACE;
                token_n->value   = -1;
                program->HEAD    += 5;
                program->c_col   += 5;
                reserved         = true;
            }
        break;
        default:{
    check_identifier:      
            token_n->line    = program->c_row + 1;
            token_n->column  = tok_col;
            temp = ( char * ) malloc ( 3 );
            assert ( temp != NULL );
            int c = 1;
            bool identifier = false;

            program->HEAD--;
            program->c_col--;
            // preparacao do identificador
            if ( program->HEAD < program->program_size && 
                 isalpha(program->source[program->HEAD]) )
            {
               identifier = true;
               temp[0] = program->source[program->HEAD++];
               program->c_col++;
               temp[1] = '\0';
            }

            // capta todo o identificador contido no source
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
                if ( program->source[program->HEAD] == ':') // rotulo
                {
                    temp = ( char * ) realloc ( temp, c + 2 );
                    temp[c++] = ':';
                    temp[c] = '\0';
                    token_n->token   = temp;
                    token_n->formal  = strdup(temp);
                    token_n->defined = true;
                    token_n->value   = -1;
                    token_n->type    = TOK_LABEL;
                    program->HEAD++;
                    program->c_col++;
                    if ( LOCAL_LABEL ) // controle do macroprocessador
                    {
                        char *prefix = malloc(3 + strlen(temp) );
                        strcpy( prefix, "%%" ); 
                        strcat( prefix, temp );
                        token_n->token  = prefix;
                        token_n->formal = strdup(prefix);
                        free(temp);
                        token_n->type  = TOK_LOCAL_LABEL;
                    }
                }else //identificador
                {
                    token_n->token   = temp;
                    token_n->formal  = strdup(temp);
                    token_n->defined = false;
                    token_n->value   = -1;
                    token_n->type    = TOK_IDENTIFIER;
                    if ( LOCAL_LABEL ) 
                    {
                        char *prefix = malloc(3 + strlen(temp) );
                        strcpy( prefix, "%%" ); 
                        strcat( prefix, temp );
                        token_n->token = prefix;
                        token_n->token = strdup(prefix);
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

    token_n->line    = program->c_row + 1;
    token_n->column  = tok_col;
    token_n->formal  = NULL;
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
    size_t len = strlen(str);
    if ( len > 0 ) 
    {
        str[ len - 1 ] = '\0'; 
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
  
    int rv;
    int pc = 0;  // posicao de memoria do programa
    int dr_idx = 0; // posicao na regiao de dados
                    
    while ( program->token_idx < program->n_tokens ) 
    {
        switch( tok->type ) 
        {
            case TOK_LABEL: {

                n_tok = malloc( sizeof( token_t ) );
                n_tok->data_l = false;
                u_tok = peek_token( program );
                // verifica se e a definicao de uma variavel
                if ( u_tok->type == TOK_WORD ||
                     u_tok->type == TOK_SPACE ) 
                {
                    n_tok->value = data_reg + (dr_idx++);

                    n_tok->token = malloc( strlen( tok->token ) + 1 );
                    strcpy( n_tok->token, tok->token);
                    cut_last( n_tok->token );

                    n_tok->defined = true;
                    n_tok->data_l  = true;
                    n_tok->type    = TOK_IDENTIFIER;
                }else if ( u_tok->type == TOK_ASCIIZ ) 
                {
                }else // se for rotulo para desvio de execucao ou variavel
                {
                    n_tok->value = pc + TEXT_SEGMENT_START;
                    n_tok->token = malloc( strlen( tok->token ) + 1 );
                    n_tok->token = strcpy( n_tok->token, tok->token );
                    cut_last( n_tok->token );
                    n_tok->defined = true;
                    n_tok->type = TOK_IDENTIFIER;
                }

                token_t *t;
                HASH_FIND_STR( program->table->tokens, n_tok->token, t );
                if ( t == NULL )
                    HASH_ADD_STR( program->table->tokens, token, n_tok );
                else
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
            case TOK_BR:
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

    // prepara as secoes do codigo objeto .elf
    Vector *dot_text = malloc(sizeof(Vector));
    initVector( dot_text, 10 ); 

    Vector *dot_data = malloc(sizeof(Vector));
    initVector( dot_data, 3 ); 

    Vector *dot_rodata = malloc(sizeof(Vector));
    initVector( dot_rodata, 2 ); 

    // prepara a struct de secoes no program
    program->sections = 
        ( sections_t * ) malloc( sizeof( sections_t ) );

    GtkTextView *console_errors = 
        GTK_TEXT_VIEW(gtk_builder_get_object(p_builder, "consoleErros" ));

    GtkTextBuffer *console_ebuffer =
        gtk_text_view_get_buffer( console_errors );

    cpe = console_ebuffer;
    int rv = EXIT_SUCCESS;

    //atribui as secoes na struct sections dentro de program
    program->sections->dot_data   = dot_data;
    program->sections->dot_text   = dot_text;
    program->sections->dot_rodata = dot_rodata;

    // cria symbol_table e prepara para receber tokens de simbolos
    symbol_table_t *table = 
        malloc ( sizeof ( symbol_table_t ) );
    
    program->table = table;
    program->table->num_s  = 0;
    program->table->tokens = NULL;

    int res = resolveIdentifiers(program);
    if ( res <= -1 ) 
        return;

    program->table->num_s = HASH_COUNT(program->table->tokens);
    program->token_idx = 0; //cabecote que percorre o vetor de tokens

    token_t *tok =
        getNextToken( program );

    //percorre tokens de program para resolver enderecamento
    while ( program->token_idx < program->n_tokens ) 
    {
        switch ( tok->type ) 
        {
            case TOK_LOAD:
                rv = parseLoad(program, tok);
            break;
            case TOK_STORE:
                rv = parseStore(program, tok);
            break;
            case TOK_WORD:
                rv = parseWord(program, tok);
            break;
            case TOK_SPACE:
                   insert( program->sections->dot_data, 0 ); 
            break;
            case TOK_STOP:
                   insert( program->sections->dot_text, STOP ); 
            break;
            case TOK_BR:
                rv = parseBR(program, tok);
            break;
            case TOK_BRNEG:
                rv = parseBRNEG(program, tok);
            break;
            case TOK_BRZERO:
                rv = parseBRZERO(program, tok);
            break;
            case TOK_BRPOS:
                rv = parseBRPOS(program, tok);
            break;
            case TOK_ADD:
                rv = parseADD(program, tok);
            break;
            case TOK_SUB:
                rv = parseSUB(program, tok);
            break;
            case TOK_READ:
                rv = parseRead(program, tok);
            break;
            case TOK_PUT:
                rv = parsePUT(program, tok);
            break;
            case TOK_DIVIDE:
                rv = parseDIVIDE(program, tok);
            break;
            case TOK_MULT:
                rv = parseMULT(program, tok);
            break;
            case TOK_CALL:
                rv = parseCALL(program, tok);
            break;
            case TOK_RET:
               insert( program->sections->dot_text, RET ); 
            break;

            case TOK_GLOBAL:
                rv = parseGLOBAL(program, tok);
            break;
            
            case TOK_EXTERN:
                rv = parseEXTERN(program, tok);
            break;

            default:
            break;

        }

        if ( rv <= -1 ) 
        {
            gtk_text_buffer_set_text(cpe, current_parser_error, -1);
            return;
        }

        tok = getNextToken(program);
    }

    program->n_globals = HASH_COUNT( program->globals );
    program->n_externs = HASH_COUNT( program->externs );

}

