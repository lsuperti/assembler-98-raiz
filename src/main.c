
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include "maquina.h"
#include "types.h"
#include "screens.h"

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
    memory[527] = 20;
    memory[528] = data_reg + 2;
    memory[529] = 00;
    memory[530] = data_reg + 45;
    memory[531] = 11;

    memory[600] = 15;
    memory[601] = data_reg + 200;
    memory[602] = 11;

    memory[455] = 12;
    memory[456] = data_reg + 221;
    memory[457] = 3;
    memory[458] = data_reg + 221;
    memory[459] = 16;

    memory[data_reg + 200] = 455;

    memory[data_reg + 13] = 321;
    memory[data_reg + 14] = 421;
    memory[data_reg + 15] = 521;
    memory[data_reg + 70] = 40;
    memory[data_reg + 100] = 50;
    memory[data_reg + 45] = 600;

    // Copies the strings to the data_reg buffer, including the null terminator
    memcpy(&memory[data_reg + 35], "negative", 9);
    memcpy(&memory[data_reg + 3], "zero", 5);
    memcpy(&memory[data_reg + 16], "positive", 9);

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

    user_data_t data = { .store = store, .builder = builder,
                        .treeview = GTK_TREE_VIEW(memory_tree) };

    g_signal_connect(gtk_builder_get_object(builder, "step")
            ,"clicked", G_CALLBACK(step), &data);
    g_signal_connect(gtk_builder_get_object(builder, "reset")
            ,"clicked", G_CALLBACK(reset), builder);
    gtk_builder_connect_signals(builder, NULL);
    // -- // 
    
    gtk_style_context_add_provider_for_screen(screen,
            GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    ri = memory[program_counter];
    update_inst_pc(builder, memory[program_counter]);

    GtkWidget *container_window = GTK_WIDGET(gtk_builder_get_object(builder,
            "CONTAINER_WINDOW"));

    GtkStack *stack = GTK_STACK(gtk_stack_new());
    gtk_stack_set_transition_type(stack,
            GTK_STACK_TRANSITION_TYPE_CROSSFADE);
    gtk_stack_set_transition_duration(stack, 100);
    gtk_container_add(GTK_CONTAINER(container_window), GTK_WIDGET(stack));

    GtkWidget *start_window_fixed =
        GTK_WIDGET(gtk_builder_get_object(builder, "WINDOW_FIXED_START"));
    GtkWidget *window_fixed_mop0 =
        GTK_WIDGET(gtk_builder_get_object(builder, "WINDOW_FIXED_MOP0"));
    GtkWidget *window_fixed_mop1 =
        GTK_WIDGET(gtk_builder_get_object(builder, "WINDOW_FIXED_MOP1"));
    GtkWidget *window_fixed_mop2 =
        GTK_WIDGET(gtk_builder_get_object(builder, "WINDOW_FIXED_MOP2"));

    gtk_stack_add_titled(stack, start_window_fixed, "WINDOW_FIXED_START", "Main window"); 
    gtk_stack_add_titled(stack, window_fixed_mop0, "WINDOW_FIXED_MOP0", "Terminal");
    gtk_stack_add_titled(stack, window_fixed_mop1, "WINDOW_FIXED_MOP1", "GUI");
    gtk_stack_add_titled(stack, window_fixed_mop2, "WINDOW_FIXED_MOP2", "Debugger");

    // On click change to console screen
    GtkWidget *console_button = GTK_WIDGET(gtk_builder_get_object(builder, "Console"));
    g_signal_connect(console_button, "clicked", G_CALLBACK(change_to_console), stack); 

    GtkWidget *gui_button = GTK_WIDGET(gtk_builder_get_object(builder, "GUI"));
    g_signal_connect(gui_button, "clicked", G_CALLBACK(change_to_gui), stack);

    GtkWidget *debugger_button = GTK_WIDGET(gtk_builder_get_object(builder, "Debugger"));
    g_signal_connect(debugger_button, "clicked", G_CALLBACK(change_to_debugger), stack); 

    g_signal_connect(container_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(container_window);
    gtk_main();

    return EXIT_SUCCESS;

}

