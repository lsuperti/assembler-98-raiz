
#include "ligador.h"

void process_section( char *section, Vector *vector )
{
    word_t value;
    char *line_copy = strdup(section);
    char *current = line_copy;
    char *next_space;

    while (*current) {
        next_space = strchr(current, ' ');

        if (next_space == NULL) {
            next_space = current + strlen(current);
        }

        *next_space = '\0';

        if (sscanf(current, "%u", &value) == 1) {
            insert(vector, value);
        }

        current = next_space + 1;
    }
}

void process_global( char *section, global *gls )
{
    word_t value;
    char *line_copy = strdup(section);
    char *current = line_copy;
    char *next_space;

    while (*current) {
        next_space = strchr(current, ' ');

        if (next_space == NULL) {
            next_space = current + strlen(current);
        }

        *next_space = '\0';

        // if (sscanf(current, "%u", &value) == 1) {
        //     insert(vector, value);
        // }

        current = next_space + 1;
    }
}

modulo *read_modulo( char *src )
{
    modulo *mod = (modulo *)malloc(sizeof(modulo));
    if (!mod) {
        fprintf(stderr, "Erro de alocação de memória\n");
        return NULL;
    }

    initVector(&mod->dot_text, 10);
    initVector(&mod->dot_data, 10);
    initVector(&mod->dot_rodata, 10);
    mod->gls = NULL;
    mod->num_gls = 0;

    bool save_text = false;
    bool save_data = false;
    bool save_rodata = false;
    bool save_global = false;
    bool save_extern = false;

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
        } else if (strstr(section, "section .data") != NULL) {
            save_data = true;

            save_text = false;
            save_rodata = false;
            save_global = false;
            save_extern = false;
        } else if (strstr(section, "section .rodata") != NULL) {
            save_rodata = true;

            save_text = false;
            save_data = false;
            save_global = false;
            save_extern = false;
        } else if (strstr(section, "global") != NULL) {
            save_global = true;

            save_rodata = false;
            save_text = false;
            save_data = false;
            save_extern = false;
        } else if (strstr(section, "extern") != NULL) {
            save_extern = true;

            save_rodata = false;
            save_text = false;
            save_data = false;
            save_global = false;
        } else {

            if (save_text) {
                process_section(section, &mod->dot_text);
            } else if (save_data) {
                process_section(section, &mod->dot_data);
            } else if (save_rodata) {
                process_section(section, &mod->dot_rodata);
            } else if (save_global) {
                process_global(section, mod->gls);
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
int first_pass( paths *p, modulo *mds, tlb_g *gs )
{
    int *err;
    size_t *f_size;
    int idx = 1;
    
    modulo *f_m;
    HASH_FIND_INT(mds, "1", f_m);
    if ( f_m == NULL )
        return -1;

    int s_data_size = f_m->dot_data.used;
    int s_text_size = f_m->dot_text.used;

    for ( GList *l = p->file_paths; l != NULL; l = l->next ) 
    {
        char *src = c_read_file( (const char*) l->data, err, f_size);
        if ( err != FILE_OK ) 
             return *err;
        modulo *m = read_modulo(src);
        
        if ( m != NULL )
        {
            m->id = idx++;
            HASH_ADD_INT(mds, id, m);
        }

        global *el, *tmp;
        HASH_ITER(hh, m->gls, el, tmp) 
        {
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
                 
        s_data_size += m->dot_data.used;
        s_text_size += m->dot_text.used;

    }

    return EXIT_SUCCESS;
}

int second_pass()
{
    return EXIT_SUCCESS;
}

void on_link_activate( GtkMenuItem *m, gpointer data ) 
{
    paths *p = data;
    int rv;
    modulo *mds = NULL;
    tlb_g  *gs  = NULL;

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
        f_m->id = 1;
        HASH_ADD_INT(mds, id, f_m);

        global *el, *tmp;
        HASH_ITER(hh, f_m->gls, el, tmp) 
        {
            global *t; 
            HASH_FIND_STR(gs->gls, el->name, t);
            if ( t == NULL )
            {
                HASH_ADD_STR(gs->gls, name, el);
            }
            else
            {
                // Error multiple defined linked
                // libraries
                return;
            }
            
        }
    }

    // Depois da primeira passagem todos os simbolos
    // com os endereços corretos estão em gs.
    rv = first_pass(p, mds, gs); 
    if (rv) 
      return;

    rv = second_pass();
    if (rv)
      return;

    on_load_activate(NULL);
}

void on_addmod_activate( GtkMenuItem *m, gpointer data )
{
    paths *p = data;

    GtkWidget *dialog = gtk_file_chooser_dialog_new("Select a File",
                                                    NULL,
                                                    GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_Open", GTK_RESPONSE_ACCEPT,
                                                    NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {

        char *file_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        p->file_paths = g_list_append(p->file_paths, file_path);

        gtk_combo_box_text_append_text(p->combo_box,
                g_path_get_basename(file_path));

        gtk_combo_box_set_active(GTK_COMBO_BOX(p->combo_box),
                                 g_list_length(p->file_paths) - 1);
    }

    gtk_widget_destroy(dialog);
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
