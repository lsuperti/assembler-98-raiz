
#include "screens.h"

void change_to_console(GtkWidget *widget, GtkStack *stack) {
    gtk_stack_set_visible_child_name(stack, "WINDOW_FIXED_MOP0" );
    mop = MOP0;
}

void change_to_gui(GtkWidget *widget, GtkStack *stack){
    gtk_stack_set_visible_child_name(stack, "WINDOW_FIXED_MOP2" );
    mop = MOP2;
}

void change_to_main(GtkWidget *widget, GtkStack *stack){
    gtk_stack_set_visible_child_name(stack, "WINDOW_FIXED_START" );
    mop = MOP_NOT_DEFINED;
}

void change_to_file_gui(GtkWidget *widget, GtkStack *stack){
    gtk_stack_set_visible_child_name(stack, "WINDOW_FIXED_MOP3" );
    mop = MOP3;
}

/**
 * Esta funcao analisa o texto editado no console2 (arquivo glade) da janela File,
 * cria um novo programa (struct program_t)...
 */
void assemble_and_update_file_gui(GtkWidget *widget, gpointer data) 
{
    user_data_t *user_data_t = data;  
    program_t *program 
        = malloc( sizeof( program_t ) ); 

    // prepara os consoles da janela File para receberem textos
    GtkWidget *console_errors =
        GTK_WIDGET(gtk_builder_get_object(user_data_t->builder,
                   "consoleErros"));
    gtk_text_buffer_set_text( 
            gtk_text_view_get_buffer( GTK_TEXT_VIEW(console_errors) ), "", -1 );

    GtkWidget *console_source =
        GTK_WIDGET(gtk_builder_get_object(user_data_t->builder,
                   "console2"));
    GtkTextBuffer *c_buff =
        gtk_text_view_get_buffer(GTK_TEXT_VIEW(console_source));  

    /* 
     * Prepara o texto que esta contido em console2 (arquivo glade).
     * O texto do arquivo pode ser alterado nesse console, entao cria-se
     * um novo programa. A partir desse novo programa o codigo alvo e montado
     */
    gchar *source;
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(c_buff, &start, &end);
    source = gtk_text_buffer_get_text(c_buff, &start, &end, FALSE);

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
    process_macros(program);
    parse(program);
    FILE *o = fopen( current_binary, "w" );
    generateOutput( program, o );
    fclose(o);

    GtkWidget *console_assembled_files =
        GTK_WIDGET(gtk_builder_get_object(user_data_t->builder,
                   "consoleAssembledFiles"));

    on_sMontador_activate(NULL, GTK_TEXT_VIEW(console_assembled_files));
    freeProgram(program);
}

