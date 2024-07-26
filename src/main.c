
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include "maquina.h"

void onDestroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

int main (int argc, char *argv[]) {

  memory[45] = 1;
  memory[4095] = 7;

  gtk_init(&argc, &argv);

  GtkBuilder *builder = gtk_builder_new_from_file("../glade_files/ui.glade");

  GtkCssProvider *provider = gtk_css_provider_new();
  GdkDisplay *display = gdk_display_get_default();
  GdkScreen *screen = gdk_display_get_default_screen(display);

  GError *error = NULL;
  gtk_css_provider_load_from_path(provider, "../styles/global.css", &error);
  if (error) {
      g_printerr("Error loading CSS: %s\n", error->message);
      g_clear_error(&error);
  }

  // ------------------------------------------ //
 
  // -- Load signals and callbacks
  gtk_builder_add_callback_symbols(builder, 
                                   "onDestroy", G_CALLBACK(onDestroy),
                                   NULL);
  g_signal_connect(gtk_builder_get_object(builder, "step")
          ,"clicked", G_CALLBACK(step), builder);
  g_signal_connect(gtk_builder_get_object(builder, "reset")
          ,"clicked", G_CALLBACK(reset), builder);
  gtk_builder_connect_signals(builder, NULL);
  // -- // 
 
  gtk_style_context_add_provider_for_screen(screen,
          GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

  char cma_str[100], cmv_str[100];  
  sprintf(cma_str, "%d", CURRENT_MEMORY_ADDRESS);
  sprintf(cmv_str, "%d", memory[CURRENT_MEMORY_ADDRESS]);
  gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,
                  "CURRENT_MEMORY_VALUE")), cmv_str);
  gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,
                  "CURRENT_MEMORY_ADDRESS")), cma_str);

  GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
  gtk_widget_show_all(window);
  gtk_main();

  return EXIT_SUCCESS;

}

