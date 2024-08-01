#ifndef HELPER_H
#define HELPER_H

#include <gtk/gtk.h>

#define DEFAULT_CAPACITY 10

void append_text_to_text_view(GtkTextView *text_view, const char *text); 
int16_t show_number_input_dialog(GtkWidget *widget, gpointer window);
#define function_t(ignore) (TRUE)

#endif // HELPER_H
