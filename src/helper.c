
#include <stdint.h>
#include "helper.h"
#include "types.h"

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

