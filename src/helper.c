
#include <stdint.h>
#include "helper.h"
#include "architecture.h"
#include "types.h"
#include <ctype.h>

/*
 * hash implementation that uses the 
 * djb2 algorithm. 
 * http://www.cse.yorku.ca/~oz/hash.html
*/

void sync_scroll(GtkAdjustment *adjustment, GtkAdjustment *synced_adjustment) {
    gtk_adjustment_set_value(synced_adjustment, gtk_adjustment_get_value(adjustment));
}

void update_line_numbers(GtkTextBuffer *buffer, GtkTextView *line_number_view) {
    GtkTextIter iter;
    gint line_count;
    gchar *line_numbers = NULL;
    GString *line_string = g_string_new("");

    // Get the total number of lines in the buffer
    line_count = gtk_text_buffer_get_line_count(buffer);

    // Create a string for the line numbers
    for (gint i = 1; i <= line_count; i++) {
        g_string_append_printf(line_string, "%d\n", i);
    }

    // Set the line number string to the line number view buffer
    GtkTextBuffer *line_buffer = gtk_text_view_get_buffer(line_number_view);
    gtk_text_buffer_set_text(line_buffer, line_string->str, -1);

    // Free the allocated memory
    g_string_free(line_string, TRUE);
}

unsigned long
hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

void colorize_token(GtkTextBuffer *buffer,
        int token_start, int token_end, const char *color) {
    GtkTextIter start_iter, end_iter;

    gtk_text_buffer_get_iter_at_offset(buffer, &start_iter, token_start);
    gtk_text_buffer_get_iter_at_offset(buffer, &end_iter, token_end);

    GtkTextTag *tag =
        gtk_text_buffer_create_tag(buffer, NULL, "foreground", color, NULL);

    gtk_text_buffer_apply_tag(buffer, tag, &start_iter, &end_iter);
}

void on_sMacros_activate( GtkMenuItem *m, GtkTextView *s )
{
    FILE *file;
    if ( (file = fopen(current_macro_out, "rb")) == NULL )
        return;

    program_t *p;
    if ( (p = createProgram(file, current_macro_out)) == NULL )
        return;
    fclose(file);

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(s));

    gtk_text_buffer_set_text( buffer,
    p->source, -1);
    tokenize(p);

    for( int i=0; i < p->n_tokens - 1; i++ )
    {
        int type = p->tokens[i].type;
        const char *color = tok_colors[type];

        if ( color != NULL )
        {
            // Offset coloring is broken currently
            // idk why, a fix is to subtract one from the first argument
            // p->tokens[i].offset - 1
            // but that is not wanted.
            if ( p->tokens[i].offset - 1 > 0 )
                colorize_token( buffer, p->tokens[i].offset - 1, 
                        p->tokens[i].offset + strlen(p->tokens[i].token),
                        color );
            else 
                colorize_token( buffer, p->tokens[i].offset, 
                        p->tokens[i].offset + strlen(p->tokens[i].token),
                        color );
        }
    }

    freeProgram(p);
}

void on_sMontador_activate( GtkMenuItem *m, GtkTextView *s )
{
    int err;
    size_t f_size;
    char *f_data = c_read_file(current_binary, &err, &f_size);
    if (err)
    {
        fprintf(stderr, "Couldn't show output from assembler");
        return;
    }
    else {
        gtk_text_buffer_set_text( 
        gtk_text_view_get_buffer(GTK_TEXT_VIEW(s)),
        f_data, -1);
        free(f_data);
    }
}

void append_text_to_text_view(GtkTextView *text_view, const char *text){
    GtkTextBuffer *buffer;
    GtkTextIter end;
    buffer = gtk_text_view_get_buffer(text_view);
    gtk_text_buffer_get_end_iter(buffer, &end);
    gtk_text_buffer_insert(buffer, &end, text, -1);
}

word_t show_number_input_dialog(GtkWidget *widget, gpointer window) {
    GtkWidget *dialog, *content_area, *spin_button;
    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    dialog = gtk_dialog_new_with_buttons("Enter a Number",
                                         GTK_WINDOW(window),
                                         flags,
                                         "_OK",
                                         GTK_RESPONSE_OK,
                                         NULL);

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    spin_button = gtk_spin_button_new_with_range( -32768, 32767, 5);
    gtk_container_add(GTK_CONTAINER(content_area), spin_button);

    gtk_widget_show_all(dialog);
    
    word_t response; 
    do { response = gtk_dialog_run(GTK_DIALOG(dialog)); }
    while ( response != GTK_RESPONSE_OK );
    
    word_t input_number =
        gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_button));
    gtk_widget_destroy(dialog);
    return input_number;
}

 void get_scroll_position(GtkScrolledWindow *scrolled_window,
         gdouble *vscroll, gdouble *hscroll) {

    GtkAdjustment *vadjustment = gtk_scrolled_window_get_vadjustment(scrolled_window);
    GtkAdjustment *hadjustment = gtk_scrolled_window_get_hadjustment(scrolled_window);

    *vscroll = gtk_adjustment_get_value(vadjustment);
    *hscroll = gtk_adjustment_get_value(hadjustment);

}

void set_scroll_position(GtkScrolledWindow *scrolled_window, gdouble vscroll, gdouble hscroll) {
    GtkAdjustment *vadjustment = gtk_scrolled_window_get_vadjustment(scrolled_window);
    GtkAdjustment *hadjustment = gtk_scrolled_window_get_hadjustment(scrolled_window);

    gtk_adjustment_set_value(vadjustment, vscroll);
    gtk_adjustment_set_value(hadjustment, hscroll);
}

void setup_tree_view(GtkWidget *treeview, GtkCellRenderer *renderer,
        GtkTreeViewColumn *address_column) {
    
    GtkTreeViewColumn *value_column;

    renderer = gtk_cell_renderer_text_new();
    address_column = gtk_tree_view_column_new_with_attributes(
            "Address", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), address_column);

    GtkWidget *address_column_button =
       gtk_tree_view_column_get_button(address_column);
    gtk_widget_set_name( address_column_button, "address_column");

    value_column = gtk_tree_view_column_new_with_attributes(
            "Value", renderer, "text", 1, NULL);

    GtkWidget *value_column_button =
       gtk_tree_view_column_get_button(value_column);
    gtk_widget_set_name( value_column_button, "value_column");

    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), value_column);
}

void onDestroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

int is_copy(int inst) {
    if (    inst == COPY_DD 
         || inst == COPY_DI
         || inst == COPY_DIm
         || inst == COPY_ID  
         || inst == COPY_II 
         || inst == COPY_IIm ) {
        return TRUE;
    }
    return FALSE;
}

int is_empty_line(const char *line) {
    while (*line != '\0') {
        if (!isspace((unsigned char)*line)) {
            return 0; 
        }
        line++;
    }
    return 1;  
}

char * c_read_file(const char * f_name, int * err, size_t * f_size) {
    char * buffer;
    size_t length;
    FILE * f = fopen(f_name, "rb");
    size_t read_length;
    
    if (f) {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        
        // 1 GiB; best not to load a whole large file in one string
        if (length > 1073741824) {
            *err = FILE_TOO_LARGE;
            
            return NULL;
        }
        
        buffer = (char *)malloc(length + 1);
        
        if (length) {
            read_length = fread(buffer, 1, length, f);
            
            if (length != read_length) {
                 free(buffer);
                 *err = FILE_READ_ERROR;

                 return NULL;
            }
        }
        
        fclose(f);
        
        *err = FILE_OK;
        buffer[length] = '\0';
        *f_size = length;
    }
    else {
        *err = FILE_NOT_EXIST;
        
        return NULL;
    }
    
    return buffer;
}

token_t * tokdup( token_t *t )
{
    if (t == NULL) 
      return NULL;

    token_t *t2;
    if ( (t2 = malloc( sizeof( token_t ) ) ) == NULL )
          return NULL;

    t2->column  = t->column;
    t2->line    = t->line;
    t2->offset  = t->offset;
    t2->defined = t->defined;
    t2->type    = t->type;
    t2->value   = t->value;

    char *t_tok;
    if ( (t_tok = strdup(t->token)) == NULL )
          return NULL;

    t2->token   = t_tok;

    char *t_form;
    if ( (t_form = strdup(t->formal)) == NULL )
          return NULL;

    t2->formal  = t_form;

    t2->pos     = *dupVector( &t->pos );

    return t2;

}

void set_combo_box_ellipsize(GtkComboBoxText *combo_box) {
    GList *renderers =
        gtk_cell_layout_get_cells(GTK_CELL_LAYOUT(combo_box));
    if ( renderers != NULL )
    {
        GtkCellRenderer *renderer = GTK_CELL_RENDERER(renderers->data);
        g_object_set(renderer, "ellipsize", PANGO_ELLIPSIZE_END, NULL);
        g_list_free(renderers);
    }
}

