
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include "helper.h"
#include "maquina.h"
#include "types.h"
#include "screens.h"
#include <fontconfig/fontconfig.h>
#include "program.h"
#include "montador.h"
#include "ligador.h"
#include "program.h"

#define NDEBUG

void cleanup() 
{
    FcFini();
    g_print("Application cleanup complete.\n");
    gtk_main_quit();
}

int main (int argc, char *argv[]) {

    /*
    * inicializacao do GTK, carga do layout do arquivo glade e css
    * criacao de variaveis para manipular treeview da memoria da maquina hipotetica
    */
    gtk_init(&argc, &argv);
    
    GtkBuilder *builder = gtk_builder_new_from_file("../glade_files/ui.glade");
    p_builder = builder;
    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);
    GtkWidget *memory_tree =
        GTK_WIDGET(gtk_builder_get_object(builder, "memory_tree"));
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    GtkListStore *store; //modelo de dados tabular
    GtkTreeIter iter;


    GError *error = NULL;
    gtk_css_provider_load_from_path(provider, "../styles/global.css", &error);
    if (error) {
        g_printerr("Error loading CSS: %s\n", error->message);
        g_clear_error(&error);
    }

    GMainContext *main_thread = g_main_context_new();
    // ------------------------------------------ //
    /*
    * configura treeview, cria tabela com colunas de posicao de memoria e de valor
    * preenche a memoria, seta o modelo na GUI e registra sinal para callback
    */

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

    /*
    * estrutura para compartilhar a exibicao da memoria da maquina hipotetica
    * callbacks para botões e conexão com sinais declarados no glade com as funcoes do codigo
    * botoes de acao em todas as janelas: run, step, reset, files, assemble
    */

    user_data_t data = { .store = store, .builder = builder,
                        .treeview = GTK_TREE_VIEW(memory_tree) };

    g_signal_connect(gtk_builder_get_object(builder, "run")
            ,"clicked", G_CALLBACK(run), &data);
    g_signal_connect(gtk_builder_get_object(builder, "run1")
            ,"clicked", G_CALLBACK(run), &data);
    g_signal_connect(gtk_builder_get_object(builder, "step")
            ,"clicked", G_CALLBACK(step), &data);
    g_signal_connect(gtk_builder_get_object(builder, "reset")
            ,"clicked", G_CALLBACK(reset), &data);
    g_signal_connect(gtk_builder_get_object(builder, "reset1")
            ,"clicked", G_CALLBACK(reset), builder);
    g_signal_connect(gtk_builder_get_object(builder, "file_button")
            ,"file-set", G_CALLBACK(open_file), builder);
    g_signal_connect(gtk_builder_get_object(builder, "assemble")
            ,"clicked", G_CALLBACK(assemble_and_update_file_gui), &data);
    
    gtk_builder_connect_signals(builder, NULL);

    // aplicando estilo css
    gtk_style_context_add_provider_for_screen(screen,
            GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    // atualiza interface para mostrar instrucao atual
    ri = memory[program_counter];
    update_inst_pc(builder, memory[program_counter]);

    GtkWidget *load = GTK_WIDGET(gtk_builder_get_object(builder, "load"));
    g_signal_connect(load, "activate", G_CALLBACK(on_load_activate), &data);
    g_signal_connect(load, "activate", G_CALLBACK(reset), &data);
    /*
    * cria conteiner baseado no glade, cria pilha de widgets, 
    * seta transicao e tempo ao alternar janelas, adiciona pilha ao conteiner
    */

    GtkWidget *container_window = GTK_WIDGET(gtk_builder_get_object(builder,
            "CONTAINER_WINDOW"));

    GtkStack *stack = GTK_STACK(gtk_stack_new());
    gtk_stack_set_transition_type(stack,
            GTK_STACK_TRANSITION_TYPE_CROSSFADE);
    gtk_stack_set_transition_duration(stack, 100);
    gtk_container_add(GTK_CONTAINER(container_window), GTK_WIDGET(stack));

    /*
    * recupera os widgets do glade: pagina inicial, terminal, GUI, file_gui
    * empilha os widgets, associa a um titulo
    */

    GtkWidget *start_window_fixed =
        GTK_WIDGET(gtk_builder_get_object(builder, "WINDOW_FIXED_START"));
    GtkWidget *window_fixed_mop0 =
        GTK_WIDGET(gtk_builder_get_object(builder, "WINDOW_FIXED_MOP0"));
    GtkWidget *window_fixed_mop2 =
        GTK_WIDGET(gtk_builder_get_object(builder, "WINDOW_FIXED_MOP2"));
    GtkWidget *window_fixed_mop3 =
        GTK_WIDGET(gtk_builder_get_object(builder, "WINDOW_FIXED_MOP3"));

    gtk_stack_add_titled(stack, start_window_fixed,
            "WINDOW_FIXED_START", "Main window"); 
    gtk_stack_add_titled(stack, window_fixed_mop0,
            "WINDOW_FIXED_MOP0", "Terminal");
    gtk_stack_add_titled(stack, window_fixed_mop2,
            "WINDOW_FIXED_MOP2", "GUI");
    gtk_stack_add_titled(stack, window_fixed_mop3,
            "WINDOW_FIXED_MOP3", "FILE_GUI");

    /*
    * Botoes de navegacao na gui do software: Console, GUI, File, Back (para cada janela)
    * Conexao dos botoes do arquivo glade com as callbacks para detectar evento clicked
    * Conexao com evento de fechamento da janela
    */
    GtkWidget *console_button =
        GTK_WIDGET(gtk_builder_get_object(builder, "Console"));
    g_signal_connect(console_button, "clicked", G_CALLBACK(change_to_console), stack); 
    GtkComboBoxText *combo_box =
        GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "combo_box"));

    set_combo_box_ellipsize(combo_box);
    paths p = { NULL, combo_box, builder };

    GtkWidget *link = 
        GTK_WIDGET(gtk_builder_get_object(builder, "link"));
    g_signal_connect(link, "activate", G_CALLBACK(on_link_activate), &p);
    
    GtkWidget *add    = GTK_WIDGET(gtk_builder_get_object(builder, "add"));
    GtkWidget *remove = GTK_WIDGET(gtk_builder_get_object(builder, "remove"));

    g_signal_connect(add, "activate", G_CALLBACK(on_addmod_activate), &p );
    g_signal_connect(remove, "activate", G_CALLBACK(on_removemod_activate), &p );

    GtkWidget *gui_button = GTK_WIDGET(gtk_builder_get_object(builder, "GUI"));
    g_signal_connect(gui_button, "clicked", G_CALLBACK(change_to_gui), stack);

    GtkWidget *file_gui_button =
        GTK_WIDGET(gtk_builder_get_object(builder, "FILE_GUI"));
    g_signal_connect(file_gui_button, "clicked", G_CALLBACK(change_to_file_gui), stack);

    GtkWidget *back_button = GTK_WIDGET(gtk_builder_get_object(builder, "back"));
    g_signal_connect(back_button, "clicked", G_CALLBACK(change_to_main), stack);
    GtkWidget *back1_button = GTK_WIDGET(gtk_builder_get_object(builder, "back1"));
    g_signal_connect(back1_button, "clicked", G_CALLBACK(change_to_main), stack);
    GtkWidget *back2_button = GTK_WIDGET(gtk_builder_get_object(builder, "back2"));
    g_signal_connect(back2_button, "clicked", G_CALLBACK(change_to_main), stack);

    GtkTextView *console2 = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "console2"));
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(console2));

    gulong changed_handler_id;

    changed_handler_id =
        g_signal_connect(buffer,
                "changed", G_CALLBACK(on_buffer_changed), &changed_handler_id);

    g_signal_connect(container_window, "destroy", G_CALLBACK(cleanup), NULL);

    // Torna visivel a interface e entra no loop da aplicacao GTK para processar os eventos
    gtk_widget_show_all(container_window);
    gtk_main();

    return EXIT_SUCCESS;
}

