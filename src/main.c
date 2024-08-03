
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include "maquina.h"
#include "types.h"

void onDestroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
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

int main (int argc, char *argv[]) {
 
  // Algoritmo simples que checa se 
  // a soma de dois números lidos é 
  // ZERO, POSITIVO OU NEGATIVO
  // o arquivo basic-algorithm
  // mostra os mnemônicos.

  // Injetado direto na memoria 
  // por agora
  // depois criando o montador 
  // é so dar um append na memoria.

  memory[256] = 12;
  memory[257] = data_reg;
  memory[258] = 12; 
  memory[259] = data_reg + 1;
  memory[260] = 3;
  memory[261] = data_reg;
  memory[262] = 2;
  memory[263] = data_reg + 1;
  memory[264] = 7;
  memory[265] = data_reg + 2;
  memory[266] = 4;
  memory[267] = data_reg + 13;
  memory[268] = 1;
  memory[269] = data_reg + 14;
  memory[270] = 5;
  memory[271] = data_reg + 15;
  memory[272] = 11;
  
  memory[321] = 8;
  memory[322] = data_reg + 3; 
  memory[323] = 11;

  memory[421] = 8;
  memory[422] = data_reg + 16;
  memory[423] = 11;

  memory[521] = 8;
  memory[522] = data_reg + 35;
  memory[523] = 14;
  memory[524] = data_reg + 70;
  memory[525] = 10;
  memory[526] = data_reg + 100;
  memory[227] = 00;
  memory[528] = data_reg + 45;
  memory[529] = 11;

  memory[data_reg + 13] = 321;
  memory[data_reg + 14] = 421;
  memory[data_reg + 15] = 521;
  memory[data_reg + 70] = 40;
  memory[data_reg + 100] = 50;
  memory[data_reg + 45] = 75;

  memory[data_reg + 35] = 0x656e;
  memory[data_reg + 36] = 0x6167;
  memory[data_reg + 37] = 0x6974;
  memory[data_reg + 38] = 0x6576;
  memory[data_reg + 39] = 0x0;

  memory[data_reg + 3] = 0x657a;
  memory[data_reg + 4] = 0x6f72; // "zero" em ASCII
  memory[data_reg + 5] = 0x0;

  memory[data_reg + 16] = 0x6f70; // "positivo" em ASCII
  memory[data_reg + 17] = 0x6973;
  memory[data_reg + 18] = 0x0;
  memory[data_reg + 19] = 0x6f76;
  memory[data_reg + 20] = 0x0;      // Null terminator 

  gtk_init(&argc, &argv);

  GtkBuilder *builder = gtk_builder_new_from_file("../glade_files/ui.glade");
  GtkCssProvider *provider = gtk_css_provider_new();
  GdkDisplay *display = gdk_display_get_default();
  GdkScreen *screen = gdk_display_get_default_screen(display);
  GtkWidget *memory_tree = GTK_WIDGET(gtk_builder_get_object(builder, "memory_tree"));
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  GtkListStore *store;
  GtkTreeIter iter;

  GError *error = NULL;
  gtk_css_provider_load_from_path(provider, "../styles/global.css", &error);
  if (error) {
      g_printerr("Error loading CSS: %s\n", error->message);
      g_clear_error(&error);
  }

  // ------------------------------------------ //

  setup_tree_view(memory_tree, renderer, column);

  store = gtk_list_store_new(2, G_TYPE_UINT, G_TYPE_INT);

  // Fill the list store with the memory values
  for (int i = 0; i < MEMORY_SIZE; i++) {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, i, 1, memory[i], -1);
  } 

  // Set the model for the tree view
  gtk_tree_view_set_model(GTK_TREE_VIEW(memory_tree), GTK_TREE_MODEL(store));

  // -- Load signals and callbacks
  gtk_builder_add_callback_symbols(builder, 
                                   "onDestroy", G_CALLBACK(onDestroy),
                                  NULL);

  user_data data = { .store = store, .builder = builder };

  g_signal_connect(gtk_builder_get_object(builder, "step")
          ,"clicked", G_CALLBACK(step), &data);
  g_signal_connect(gtk_builder_get_object(builder, "reset")
          ,"clicked", G_CALLBACK(reset), builder);
  gtk_builder_connect_signals(builder, NULL);
  // -- // 
 
  gtk_style_context_add_provider_for_screen(screen,
          GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

  update_inst_pc(builder, memory[program_counter]);

  GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
  gtk_widget_show_all(window);
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  gtk_main();

  return EXIT_SUCCESS;

}

