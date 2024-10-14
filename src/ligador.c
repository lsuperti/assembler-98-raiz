
#include "ligador.h"

void process_section( char *section, Vector *vector )
{
    word_t value;
    char *line_copy = strdup(section);
    char *current = line_copy;
    char *next_space;

    while ( (next_space = strchr(current, ' ')) != NULL ) {

        *next_space = '\0';

        if (sscanf(current, "%hd", &value) == 1) {
            insert(vector, value);
        }

        current = next_space + 1;
    }

    if (sscanf(current, "%hd", &value) == 1) {
        insert(vector, value);
    }
}

void process_global(char *section, global **gls, int *num_gls) {
    char *current = strdup(section);
    char *key, *value_str;
    char *next_space;

    next_space = strchr(current, ' ');
    if (next_space == NULL) {
        next_space = strchr(current, '\t');
    }

    if (next_space != NULL) {
        *next_space = '\0';
        key = strdup(current);
        value_str = strdup(next_space + 1);

        if (value_str == NULL) {
            fprintf(stderr, "Formato de linha inválido\n");
            free(current);
            return;
        }

        *gls = realloc(*gls, (*num_gls + 1) * sizeof(global));
        if (*gls == NULL) {
            fprintf(stderr, "Erro ao realocar memória\n");
            exit(1);
        }

        (*gls)[*num_gls].value = (int)strtol(value_str, NULL, 0);
        (*gls)[*num_gls].data_l = key[strlen(key) - 1] == '&';
    
        if ( (*gls)[*num_gls].data_l == true )
            key[strlen(key) - 1] = '\0';
        (*gls)[*num_gls].name = key;

        (*num_gls)++;
    } else {
        fprintf(stderr, "Formato de linha inválido\n");
    }

    free(value_str);
    free(current);
}

void process_extern(char *section, extern_t **exts, int *num_exts) {

    char *current = strdup(section);
    char *key;
    char *next_space;

    next_space = strchr(current, ' ');
    if (next_space == NULL) {
        next_space = strchr(current, '\t');
    }

    if (next_space == NULL) {
        next_space = current + strlen(current);
    }

    if (*next_space != '\0') {
        *next_space = '\0';
        key = strdup(current);

        *exts = realloc(*exts, (*num_exts + 1) * sizeof(extern_t));
        if (*exts == NULL) {
            fprintf(stderr, "Erro ao realocar memória\n");
            exit(1);
        }

        (*exts)[*num_exts].name = key;

        initVector(&(*exts)[*num_exts].ps, 10);

        current = next_space + 1;

        word_t value;
        while ( (next_space = strchr(current, ' ')) != NULL ) {

            *next_space = '\0';

            if (strlen(current) > 0 && sscanf(current, "%hu", &value) == 1) {
                insert(&(*exts)[*num_exts].ps, value);
            }

            current = next_space + 1;
        }

        if (strlen(current) > 0 && sscanf(current, "%hu", &value) == 1) {
            insert(&(*exts)[*num_exts].ps, value);
        }

        (*num_exts)++;
    }

}

void process_local(char *section, local_t **exts, int *num_exts
        , bool data_l ) {

    char *current = strdup(section);
    char *key;
    char *next_space;

    next_space = strchr(current, ' ');
    if (next_space == NULL) {
        next_space = strchr(current, '\t');
    }

    if (next_space == NULL) {
        next_space = current + strlen(current);
    }

    if (*next_space != '\0') {
        *next_space = '\0';
        key = strdup(current);

        *exts = realloc(*exts, (*num_exts + 1) * sizeof(local_t));
        if (*exts == NULL) {
            fprintf(stderr, "Erro ao realocar memória\n");
            exit(1);
        }

        (*exts)[*num_exts].name = key;

        initVector(&(*exts)[*num_exts].ps, 10);

        current = next_space + 1;

        word_t value;
        while ( (next_space = strchr(current, ' ')) != NULL ) {

            *next_space = '\0';

            if (strlen(current) > 0 && sscanf(current, "%hu", &value) == 1) {
                insert(&(*exts)[*num_exts].ps, value);
            }

            current = next_space + 1;
        }

        if (strlen(current) > 0 && sscanf(current, "%hu", &value) == 1) {
            insert(&(*exts)[*num_exts].ps, value);
        }
        
        (*exts)[*num_exts].data_l = data_l;
        (*num_exts)++;
    }

}

modulo *read_modulo( char *src )
{
    modulo *mod = (modulo *)malloc(sizeof(modulo));
    if (!mod) {
        fprintf(stderr, "Erro de alocação de memória\n");
        return NULL;
    }

    Vector *dot_text = malloc(sizeof(Vector)); 
    initVector(dot_text, 10);

    Vector *dot_data = malloc(sizeof(Vector)); 
    initVector(dot_data, 10);

    Vector *dot_rodata = malloc(sizeof(Vector)); 
    initVector(dot_rodata, 10);

    mod->dot_text  = *dot_text;
    mod->dot_data  = *dot_data;
    mod->dot_rodata= *dot_rodata;

    mod->gls = NULL;
    mod->num_gls = 0;
    
    mod->exts = NULL;
    mod->num_exts = 0;

    mod->locals = NULL;
    mod->num_locals = 0;

    bool save_text = false;
    bool save_data = false;
    bool save_rodata = false;
    bool save_global = false;
    bool save_extern = false;
    bool save_data_locals = false;
    bool save_text_locals = false;

    char *src_copy = strdup(src);
    if (src_copy == NULL) {
        fprintf(stderr, "Erro ao copiar a string src\n");
        exit(1);
    }

    char *section = strtok(src_copy, "\n");
    while (section != NULL) {

        if (strstr(section, "section .text") != NULL) {
            save_text = true;

            save_data = false;
            save_rodata = false;
            save_global = false;
            save_extern = false;
            save_data_locals = false;
            save_text_locals = false;
        } else if (strstr(section, "section .data") != NULL) {
            save_data = true;

            save_text = false;
            save_rodata = false;
            save_global = false;
            save_extern = false;
            save_data_locals = false;
            save_text_locals = false;
        } else if (strstr(section, "section .rodata") != NULL) {
            save_rodata = true;

            save_text = false;
            save_data = false;
            save_global = false;
            save_extern = false;
            save_data_locals = false;
            save_text_locals = false;
        } else if (strstr(section, "global") != NULL) {
            save_global = true;

            save_rodata = false;
            save_text = false;
            save_data = false;
            save_extern = false;
            save_data_locals = false;
            save_text_locals = false;
        } else if (strstr(section, "extern") != NULL) {
            save_extern = true;

            save_rodata = false;
            save_text = false;
            save_data = false;
            save_global = false;
            save_data_locals = false;
            save_text_locals = false;
        } else if (strstr(section, "local_data_labels") != NULL ) {
            save_data_locals = true;

            save_text = false;
            save_data = false;
            save_rodata = false;
            save_global = false;
            save_extern = false;
            save_text_locals = false;
        } else if (strstr(section, "local_text_labels") != NULL ) {
            save_text_locals = true;

            save_text = false;
            save_data = false;
            save_rodata = false;
            save_global = false;
            save_extern = false;
            save_data_locals = false;
        } else {

            if (save_text) {
                process_section(section, &mod->dot_text);
            } else if (save_data) {
                process_section(section, &mod->dot_data);
            } else if (save_rodata) {
                process_section(section, &mod->dot_rodata);
            } else if (save_global) {
                process_global(section, &mod->gls, &mod->num_gls);
            } else if (save_extern) {
                process_extern(section, &mod->exts, &mod->num_exts);
            } else if (save_data_locals) {
                process_local(section,  &mod->locals, &mod->num_locals, true);
            } else if (save_text_locals) {
                process_local(section,  &mod->locals, &mod->num_locals, false);
            }

        }

        section = strtok(NULL, "\n");
    }

    return mod;
}

void print_modulo(modulo *mod) {
    printf("\nModulo ID: %d\n", mod->id);

    printf(".text: ");
    for (size_t i = 0; i < mod->dot_text.used; i++) {
        printf("%u ", mod->dot_text.array[i]);
    }
    printf("\n");

    printf(".data: ");
    for (size_t i = 0; i < mod->dot_data.used; i++) {
        printf("%u ", mod->dot_data.array[i]);
    }
    printf("\n");

    printf(".rodata: ");
    for (size_t i = 0; i < mod->dot_rodata.used; i++) {
        printf("%u ", mod->dot_rodata.array[i]);
    }
    printf("\n");

    printf(".global: ");
    for (size_t i = 0; i < mod->num_gls; i++) {
        printf("\n%s: %i", mod->gls[i].name, mod->gls[i].value);
    }
    printf("\n");

    printf(".extern: ");
    for (size_t i = 0; i < mod->num_exts; i++) {
        printf("\n%s", mod->exts[i].name);
        for (size_t j = 0; j < mod->exts[i].ps.used; j++) {
            printf(" %i", mod->exts[i].ps.array[j]);
        }
    }
    printf("\n");

    printf(".local_text: ");
    for (size_t i = 0; i < mod->num_locals; i++) {
        if ( mod->locals[i].data_l == true )
        {
            printf("\n%s", mod->locals[i].name);
            for (size_t j = 0; j < mod->locals[i].ps.used; j++) {
                printf(" %i", mod->locals[i].ps.array[j]);
            }
        }
    }
    printf("\n");

    printf(".local_data: ");
    for (size_t i = 0; i < mod->num_locals; i++) {
        if ( mod->locals[i].data_l == false )
        {
            printf("\n%s", mod->locals[i].name);
            for (size_t j = 0; j < mod->locals[i].ps.used; j++) {
                printf(" %i", mod->locals[i].ps.array[j]);
            }
        }
    }
    printf("\n");
}

Vector find_all_identifier_pos( program_t *p, token_t *tok )
{
    Vector *rt = malloc(sizeof(Vector));
    initVector( rt, 5 );

    int pc = 0;
    for ( int i=0; i < p->n_tokens; i++ )
    {
        token_t t = p->tokens[i];
        switch ( t.type ) 
        {
                case TOK_STORE:
                case TOK_LOAD:
                case TOK_ADD:
                case TOK_SUB:
                case TOK_BRNEG:
                case TOK_BRZERO:
                case TOK_BR:
                case TOK_BRPOS:
                case TOK_CALL:
                case TOK_DIVIDE:
                case TOK_MULT:
                case TOK_READ:
                case TOK_WRITE:
                case TOK_PUT:
                case TOK_STOP: {
                    token_t pk_t;
                    if ( i + 1 < p->n_tokens )
                        pk_t = p->tokens[i + 1];

                    if ( pk_t.token != NULL )
                    {
                        if ( strcmp( tok->token, pk_t.token ) == 0 )
                            insert( rt, pc + 1);
                    }

                    pc+=2;
                    break;
                }
                case TOK_RET:
                    pc++;
                break;
                case TOK_COPY:{

                    token_t pk_t, pk_t2;
                    if ( i + 1 < p->n_tokens )
                        pk_t = p->tokens[i + 1];

                    if ( i + 2 < p->n_tokens )
                        pk_t2 = p->tokens[i + 2];

                    if ( pk_t.token  != NULL &&
                         pk_t2.token != NULL )
                    {
                        if ( strcmp( tok->token, pk_t.token ) == 0 )
                            insert( rt, pc + 1);

                        if ( strcmp( tok->token, pk_t2.token ) == 0 )
                            insert( rt, pc + 2);
                    }

                    pc+=3;
                    break;
                }
                default:
                break;
        }

    }
    return *rt;
}

void on_save_assembled_activate( GtkMenuItem *m, GtkTextView *ct ) 
{
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new("Save File",
                                         NULL,
                                         GTK_FILE_CHOOSER_ACTION_SAVE,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Save", GTK_RESPONSE_ACCEPT,
                                         NULL);

    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "Untitled.elf98");
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(ct);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename;
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        GtkTextIter start, end;
        gtk_text_buffer_get_start_iter(buffer, &start);
        gtk_text_buffer_get_end_iter(buffer, &end);

        gchar *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

        FILE *file = fopen(filename, "w");
        if (file == NULL) {
            g_printerr("Could not open file %s for writing\n", filename);
            g_free(text);
            g_free(filename);
            gtk_widget_destroy(dialog);
            return;
        }

        fprintf(file, "%s", text);

        fclose(file);
        g_free(text);
        g_free(filename);
        g_print("File saved successfully\n");
    }

    gtk_widget_destroy(dialog);
}

/* 
 * Página 180 do livro do koliver 
 * na primeira passagem é gerado a tabela de todos 
 * os simbolos globais ( global ) e somado com a
 * determinada distância relativa.
 *
 * Somando o tamanho dos dados até o modulo presente se
 * data_l = true ou somando o tamanho do código até o presente
 * se data_l = false
*/
int first_pass( paths *p, modulo *mds, tlb_g *gs, int *t_idx )
{
    int    err = 0;
    size_t f_size = 0;
    int    idx = *t_idx;
    
    modulo *f_m;
    HASH_FIND_INT(mds, &idx, f_m);
    if ( f_m == NULL )
        return -1;

    int s_data_size = f_m->dot_data.used;
    int s_text_size = f_m->dot_text.used;
    char *src = NULL;

    for ( GList *l = p->file_paths; l != NULL; l = l->next ) 
    {
        src = c_read_file( (const char *) l->data, &err, &f_size);
        if ( err ) 
             return err;
        modulo *m = read_modulo(src);
        
        if ( m != NULL )
        {

            for( int i=0; i < m->num_gls; i++ )
            {
                global *el = &m->gls[i];
                if ( el->data_l == true )
                     el->value += s_data_size;
                else 
                     el->value += s_text_size;

                global *t; 
                HASH_FIND_STR(gs->gls, el->name, t);
                if ( t == NULL )
                {
                    HASH_ADD_STR(gs->gls, name, el);
                }
                else
                {
                    // linker error :
                    // Error multiple defined identifiers
                    return -1;
                }
            }

            for( size_t i=0; i < m->num_locals; i++ )
            {
                for( size_t j=0; j < m->locals[i].ps.used; j++ ) 
                {
                    word_t addr = m->locals[i].ps.array[j];
                    if ( m->locals[i].data_l == true )
                    {
                        m->dot_text.array[addr] += s_data_size; 
                    }
                    else if ( m->locals[i].data_l == false )
                    {
                        m->dot_text.array[addr] += s_text_size; 
                    }
                }
            }

            m->id = ++idx;
            HASH_ADD_INT(mds, id, m); 

            *t_idx = idx;

        } 

        s_data_size += m->dot_data.used;
        s_text_size += m->dot_text.used;

    }

    return EXIT_SUCCESS;
}

void on_sligador_activate(GtkMenuItem *m, GtkTextView *s)
{
    int err;
    size_t size;
    char *bin = c_read_file(current_linker_out, &err, &size);
    if ( err )
    {
        return;
    }
        
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(s));
    gtk_text_buffer_set_text( buffer,
    bin, -1);
    free(bin);
}

void print_after( modulo *mod, FILE *f ) 
{
    fprintf(f,  magic );  
    fprintf(f, "\nModulo ID: %d\n", mod->id);

    fprintf(f, "section .text\n");
    for (size_t i = 0; i < mod->dot_text.used; i++) {
        fprintf(f, "%u ", mod->dot_text.array[i]);
        if ( ( i + 1 ) % 8 == 0 ) { fprintf( f, "\n" ); }
    }
    fprintf(f, "\n");

    fprintf(f, "section .data\n");
    for (size_t i = 0; i < mod->dot_data.used; i++) {
        fprintf(f, "%u ", mod->dot_data.array[i]);
    }
    fprintf(f, "\n");

    fprintf(f, "section .rodata\n");
    for (size_t i = 0; i < mod->dot_rodata.used; i++) {
        fprintf(f, "%u ", mod->dot_rodata.array[i]);
    }
    fprintf(f, "\n");
    fflush(f);
}

int second_pass( modulo *mds, tlb_g *gs, int idx )
{
    modulo *el, *tmp, *glb;
    if ( (glb = (modulo *) malloc( sizeof(modulo) )) == NULL )
        return ENOMEM;
    
    glb->id = OUTPUT_ID;

    initVector(&glb->dot_text,   10); 
    initVector(&glb->dot_data,   10); 
    initVector(&glb->dot_rodata, 10); 

    for ( size_t k=0; k < HASH_COUNT(mds); k++ )
    {
        
       modulo *el;
       HASH_FIND_INT(mds, &k, el);
       for( size_t i=0; i < el->num_exts; i++ )
       {
           for( size_t j=0; j < el->exts[i].ps.used; j++ ) 
           {
               word_t addr = el->exts[i].ps.array[j];
               global *rpl;
               HASH_FIND_STR( gs->gls, el->exts[i].name, rpl);

               if ( rpl != NULL && addr < el->dot_text.used )
                   el->dot_text.array[addr] = rpl->value;
           }
       }

       concatenate( &glb->dot_text,   &el->dot_text ); 
       concatenate( &glb->dot_data,   &el->dot_data ); 
       concatenate( &glb->dot_rodata, &el->dot_rodata); 
    }
    
    FILE *f;
    if ( (f = fopen(current_linker_out, "w")) == NULL )
        return -1;
    print_after(glb, f);
    fclose(f);

    return EXIT_SUCCESS;
}

void on_link_activate( GtkMenuItem *m, gpointer data ) 
{
    paths *p = data;
    int rv = 0;
    modulo *mds = NULL;
    tlb_g  *gs  = NULL; 
    int idx = 0;

    if ( (gs = malloc(sizeof(tlb_g))) == NULL )
          return;

    gs->gls     = NULL;

    // O primeiro modulo é sempre o 
    // que está no buffer sendo mostrado.
    
    GtkTextView *tv   =
        GTK_TEXT_VIEW(gtk_builder_get_object(p->builder, "consoleAssembledFiles")); 
    GtkTextBuffer *tb = gtk_text_view_get_buffer(tv);
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(tb, &start);
    gtk_text_buffer_get_end_iter(tb, &end);
    gchar *text = gtk_text_buffer_get_text(tb, &start, &end, FALSE);
    modulo *f_m = read_modulo(text);

    if ( f_m != NULL )
    {
        for( int i=0; i < f_m->num_gls; i++ )
        {
          global *t;
          HASH_FIND_STR(gs->gls, f_m->gls[i].name, t);
          if ( t == NULL )
          {
              HASH_ADD_STR(gs->gls, name, &f_m->gls[i]);
          }
          else 
          {
              return;
          }

        }

        f_m->id   = idx;
        HASH_ADD_INT(mds, id, f_m);
    }

    // Depois da primeira passagem todos os simbolos
    // com os endereços corretos estão em gs.
    rv = first_pass(p, mds, gs, &idx); 
    if (rv) 
      return;

    rv = second_pass(mds, gs, idx);
    if (rv)
      return;

    if ( g_list_length(p->file_paths) > 0 )
    {
        GtkTextView *c =
            GTK_TEXT_VIEW(gtk_builder_get_object( p_builder, "consoleErros" ));
        append_text_to_text_view(c, "Linked...\n");
    }
    g_list_free(p->file_paths);

}

void on_addmod_activate( GtkMenuItem *m, gpointer data )
{
    paths *p = data;

    GtkDialog *dialog = GTK_DIALOG( gtk_file_chooser_dialog_new("Select a File",
                                                    NULL,
                                                    GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_Open", GTK_RESPONSE_ACCEPT,
                                                    NULL) );
    
    if (gtk_dialog_run(dialog) == GTK_RESPONSE_ACCEPT) {

        char *file_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        p->file_paths = g_list_append(p->file_paths, file_path);

        gtk_combo_box_text_append_text(p->combo_box,
                g_path_get_basename(file_path));

        gtk_combo_box_set_active(GTK_COMBO_BOX(p->combo_box),
                                 g_list_length(p->file_paths) - 1);
    }

    gtk_widget_destroy(GTK_WIDGET(dialog));
}

void on_removemod_activate( GtkMenuItem *m, gpointer data )
{
    paths *p = data;

    int active_index = gtk_combo_box_get_active(GTK_COMBO_BOX(p->combo_box));
    if (active_index != -1) {

        GList *item = g_list_nth(p->file_paths, active_index);
        if (item) {
            g_free(item->data);
            p->file_paths = g_list_delete_link(p->file_paths, item);

            gtk_combo_box_text_remove(p->combo_box, active_index);
        }
    }
}

void on_clear_clicked ( void* m, GtkTextView *t ) 
{
    GtkTextBuffer *b = gtk_text_view_get_buffer(t);
    gtk_text_buffer_set_text(b, "", -1);
}

