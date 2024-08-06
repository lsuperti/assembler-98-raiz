#ifndef HELPER_H
#define HELPER_H

#include <gtk/gtk.h>
#include "types.h"

#define DEFAULT_CAPACITY 10
#define function_t(ignore) (TRUE)
#define WORD_LOWER_BYTE(x) ((uint8_t)(x & 0xFF))
#define WORD_UPPER_BYTE(x) ((uint8_t)( x >> 8 ) & 0xFF)

void append_text_to_text_view(GtkTextView *text_view, const char *text);
word_t show_number_input_dialog(GtkWidget *widget, gpointer window);
void get_scroll_position(GtkScrolledWindow *scrolled_window,
        gdouble *vscroll, gdouble *hscroll); 
void set_scroll_position(GtkScrolledWindow *scrolled_window,
        gdouble vscroll, gdouble hscroll); 

#endif // HELPER_H
