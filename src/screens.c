
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

void assemble_and_update_file_gui(GtkWidget *widget, gpointer data) 
{
    user_data_t *user_data_t = data;  
    program_t *program 
        = malloc( sizeof( program_t ) ); 

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
    program->c_line       = 0;
    program->n_globals    = 0;
    program->externs      = NULL;
    program->n_externs    = 0;

    tokenize(program);
    parse(program);
    FILE *o = fopen( current_binary, "w" );
    generateOutput( program, o );
    fclose(o);

    GtkWidget *console_assembled_files =
        GTK_WIDGET(gtk_builder_get_object(user_data_t->builder,
                   "consoleAssembledFiles"));

    int err;
    size_t f_size;
    char * f_data;
    f_data = c_read_file(current_binary, &err, &f_size);

    if (err) {}
    else {
        gtk_text_buffer_set_text( 
        gtk_text_view_get_buffer(GTK_TEXT_VIEW(console_assembled_files)),
        f_data, -1);
        free(f_data);
    }

    load_program();
    update_inst_pc( user_data_t->builder, memory[program_counter] );
    update_memory_tree(user_data_t);
    g_free(source);

}

