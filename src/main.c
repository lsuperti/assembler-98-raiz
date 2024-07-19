
#include <gtk/gtk.h>
#include "example.h"

static void on_activate (GtkApplication *app) {

  GtkWidget *button;
  GtkWidget *window = gtk_application_window_new (app);
  GtkWidget *fixed;

  GtkCssProvider *provider;
  GdkDisplay *display;

  gtk_window_set_title(GTK_WINDOW(window), "Assembler 98 Raiz");
  gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

  fixed = gtk_fixed_new();
  gtk_window_set_child (GTK_WINDOW (window), fixed);
  button = gtk_button_new_with_label ("Load File");
  gtk_fixed_put(GTK_FIXED(fixed), button, 200, 200);
  gtk_widget_set_size_request(button, 100, 50);

  provider = gtk_css_provider_new();
  gtk_css_provider_load_from_path(provider, "../styles/global.css"); 
  display = gdk_display_get_default();
  gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

  gtk_window_present(GTK_WINDOW(window)); 

}

int main (int argc, char *argv[]) {
  // Create a new application
  example();

  GtkApplication *app = gtk_application_new ("com.example.GtkApplication",
                                             G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (on_activate), NULL);
  return g_application_run (G_APPLICATION (app), argc, argv);
}

