
#include <gtk/gtk.h>
#include <stdio.h>
#include "example.h"
#include "global.h"

typedef struct {
    GtkFileDialog *file_dialog;
    GtkWidget *window;
    GtkTextBuffer* buffer;
} FileDialogData;

gchar* sanitize_text(const gchar* text) {

    if (g_utf8_validate(text, -1, NULL)) {
        return g_strdup(text);
    } else {
        GString* sanitized = g_string_new(NULL);
        const gchar* end;
        while (*text != '\0') {
            if (g_utf8_validate(text, -1, &end)) {
                g_string_append_len(sanitized, text, end - text);
                text = end;
            } else {
                g_string_append_c(sanitized, ' ');
                text = end + 1; 
            }
        }
        return g_string_free(sanitized, FALSE);
    }

}

void on_file_action( GObject *source_object, GAsyncResult *res, gpointer user_data){

    FileDialogData *data = ( FileDialogData * ) user_data;
    GFile *file =
        gtk_file_dialog_open_finish(GTK_FILE_DIALOG(source_object), res, NULL);

    if (file) {

        gchar *filename = g_file_get_path(file);
        gchar *content;
        gsize length;
        GError *error = NULL;

        if (g_file_get_contents(filename, &content, &length, &error)) {
            g_print("File contents:\n%s\n", content);

            char* t = g_realloc(source_buffer, length + 1);
            if ( t == NULL ) {
                perror("Failed to allocate source buffer.");
                return;
            }
            source_buffer = t;
            source_size = length + 1;
            strncpy(source_buffer, content, length + 1);
            gtk_text_buffer_set_text(
                    data->buffer, sanitize_text(source_buffer), -1);

            g_free(content);

        } else {
            g_printerr("Error reading file: %s\n", error->message);
            g_clear_error(&error);
        }

        g_free(filename);
        g_object_unref(file);

    }

}

void button_clicked(GtkWidget *button,  gpointer user_data) {

    FileDialogData *data = (FileDialogData *)user_data;

    if (data == NULL) {
        g_printerr("User data is NULL\n");
        return;
    }else if (data->file_dialog == NULL) {
        g_printerr("Dialog is NULL\n");
        return;
    }else if (data->window == NULL) {
        g_printerr("Window is NULL\n");
        return;
    }

    gtk_file_dialog_open(data->file_dialog,
        GTK_WINDOW(data->window), NULL, on_file_action, data);

}

static void on_activate (GtkApplication *app) {

  GtkWidget *load_button, *assemble_button;
  GtkWidget *fixed; 
  GtkCssProvider *provider;
  GdkDisplay *display;
  GtkWidget *text_view = gtk_text_view_new();
  GtkTextBuffer *buffer = gtk_text_view_get_buffer( GTK_TEXT_VIEW(text_view) );

  GtkWidget *window = gtk_application_window_new (app);
  gtk_window_set_title(GTK_WINDOW(window), "Assembler 98 Raiz");
  gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

  fixed = gtk_fixed_new();
  gtk_window_set_child (GTK_WINDOW (window), fixed);
  load_button = gtk_button_new_with_label ("Load File");
  assemble_button = gtk_button_new_with_label ("Assemble");

  // -- Set widgets position on window
  gtk_fixed_put(GTK_FIXED(fixed), text_view, 200, 200); 
  gtk_fixed_put(GTK_FIXED(fixed), load_button, 200, 100);
  gtk_fixed_put(GTK_FIXED(fixed), assemble_button, 350, 250);
  // -- //

  // -- Set widgets size 
  gtk_widget_set_size_request(load_button, 100, 50);
  gtk_widget_set_size_request(text_view, 100, 200);
  gtk_widget_set_size_request(assemble_button, 100, 50);
  // -- //

  // -- Set widgets classes &|| names 
  gtk_widget_set_css_classes(load_button, (const char*[]){"button", NULL} ); 
  gtk_widget_set_css_classes(assemble_button, (const char*[]){"assemble_button", NULL} );
  // -- // 
  
  // -- Open file manager on load_button click
  GtkFileDialog *file_dialog = gtk_file_dialog_new();
  FileDialogData *data = g_malloc(sizeof(FileDialogData));
  data->file_dialog = file_dialog;
  data->window = window;
  data->buffer = buffer;
  g_signal_connect (load_button, "clicked", G_CALLBACK (button_clicked), data);
  // -- // 
  
  gtk_text_buffer_set_text(buffer, source_buffer, source_size);

  // -- Set global.css on styles folder as main css
  provider = gtk_css_provider_new();
  gtk_css_provider_load_from_path(provider, "../styles/global.css"); 
  display = gdk_display_get_default();
  gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
  // -- // 

  gtk_window_present(GTK_WINDOW(window)); 

}

int main (int argc, char *argv[]) {

  example();

  GtkApplication *app = gtk_application_new ("com.example.GtkApplication",
                                             G_APPLICATION_DEFAULT_FLAGS);

  g_signal_connect (app, "activate", G_CALLBACK (on_activate), NULL);

  gint status = g_application_run (G_APPLICATION (app), argc, argv);

  // ----------------
 
  free(source_buffer);
  g_object_unref(app);

  return status;

}

