#ifndef HELPER_H
#define HELPER_H

#include <gtk/gtk.h>

#define DEFAULT_CAPACITY 10
#define function_t(ignore) (TRUE)

extern void append_text_to_text_view(GtkTextView *text_view, const char *text); 
extern int16_t show_number_input_dialog(GtkWidget *widget, gpointer window);
extern void get_scroll_position(GtkScrolledWindow *scrolled_window,
        gdouble *vscroll, gdouble *hscroll); 
extern void set_scroll_position(GtkScrolledWindow *scrolled_window,
        gdouble vscroll, gdouble hscroll); 

#endif // HELPER_H
