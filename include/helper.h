#ifndef HELPER_H
#define HELPER_H

#include <gtk/gtk.h>
#include "montador_types.h"
#include "types.h"

#define DEFAULT_CAPACITY 10
#define function_t(ignore) (TRUE)
#define WORD_LOWER_BYTE(x) ((uint8_t)(x & 0xFF))
#define WORD_UPPER_BYTE(x) ((uint8_t)( x >> 8 ) & 0xFF)

#define FILE_OK 0
#define FILE_NOT_EXIST 1
#define FILE_TOO_LARGE 2
#define FILE_READ_ERROR 3

char * c_read_file(const char * f_name, int * err, size_t * f_size);
void append_text_to_text_view(GtkTextView *text_view, const char *text);
word_t show_number_input_dialog(GtkWidget *widget, gpointer window);
void get_scroll_position(GtkScrolledWindow *scrolled_window,
        gdouble *vscroll, gdouble *hscroll); 
void set_scroll_position(GtkScrolledWindow *scrolled_window,
        gdouble vscroll, gdouble hscroll); 
void setup_tree_view(GtkWidget *treeview, GtkCellRenderer *renderer,
        GtkTreeViewColumn *address_column); 
void onDestroy(GtkWidget *widget, gpointer data); 
int is_copy(int inst);
int is_empty_line(const char *line);
unsigned long hash(unsigned char *str);
token_t * tokdup( token_t *t );
void set_combo_box_ellipsize(GtkComboBoxText *combo_box); 

#endif // HELPER_H
