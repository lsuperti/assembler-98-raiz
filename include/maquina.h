#ifndef MAQUINA_H
#define MAQUINA_H

#include <stdio.h>
#include <gtk/gtk.h>

// READ - 12
// ...

int CURRENT_MEMORY_ADDRESS = 0;
int memory[4096] = { 12, 45, 0 };  
long int accumulator;

void execute_current_instruction(void* data) {
    if ( CURRENT_MEMORY_ADDRESS <= sizeof(memory) / sizeof(memory[0]) ) {
        int inst = memory[CURRENT_MEMORY_ADDRESS];                
        switch(inst) {
            case 12:
                accumulator = memory[CURRENT_MEMORY_ADDRESS + 1];
                CURRENT_MEMORY_ADDRESS += 2;
            break;

            default:
                g_printerr( "Not an instruction at memory address : %d \n",
                        CURRENT_MEMORY_ADDRESS );                
            break;
        }
    } else  {
        g_printerr("Memory limit reached");
    }

    char prefix[] = "accum : ";
    char acm_str[100];
    snprintf(acm_str, 100, "%ld", accumulator); 
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(data, 
                    "accumulator")), strcat(prefix,  acm_str) );
}

void step(GtkWidget *widget, gpointer data) {
    execute_current_instruction(data);
    char cma_str[100], cmv_str[100];  
    snprintf(cma_str, 100, "%d", CURRENT_MEMORY_ADDRESS);
    snprintf(cmv_str, 100, "%d", memory[CURRENT_MEMORY_ADDRESS]);
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(data
                  , "CURRENT_MEMORY_VALUE")), cmv_str);
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(data,
                  "CURRENT_MEMORY_ADDRESS")), cma_str);
}

#endif // MAQUINA_H

