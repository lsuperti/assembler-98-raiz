#ifndef MAQUINA_H
#define MAQUINA_H

#include <stdio.h>
#include <gtk/gtk.h>

int CURRENT_MEMORY_ADDRESS = 511;
int memory[512] = { 3, 45, 7, 45, 0 }; 
long int accumulator;

void update_inst_pc(void* builder, int inst) {
    char pc[100], inst_str[100], operand1[100];  
    snprintf(pc, 100, "%d", CURRENT_MEMORY_ADDRESS);
    snprintf(inst_str, 100, "%d", inst);
    snprintf(operand1, 100, "%d", memory[CURRENT_MEMORY_ADDRESS + 1]);
    char prefix[100] = "PC : ";
    char store[100] = "STORE  ";
    char load[100] = "LOAD  ";
    char unknown[100] = "????  ";
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,
                      "CURRENT_MEMORY_ADDRESS")), strcat(prefix, pc) );
    switch (inst) {
        // Casos onde a instrução tem somente 1 operando
        // Ex: LOAD, STORE
        case 3: 
            gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,
                           "CURRENT_MEMORY_VALUE")),
                           strcat(load, operand1));
        break;
        case 7:
            gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,
                           "CURRENT_MEMORY_VALUE")),
                           strcat(store, operand1));
        break;
        default: 
            gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,
                           "CURRENT_MEMORY_VALUE")),
                           strcat(unknown, operand1));
        break;
    }
}

void reset(GtkWidget *widget, gpointer builder) {
    CURRENT_MEMORY_ADDRESS = 0;
    update_inst_pc(builder, memory[CURRENT_MEMORY_ADDRESS]);
}

void execute_current_instruction(void* data) {
    size_t memory_len = sizeof(memory) / sizeof(memory[0]);
    if ( CURRENT_MEMORY_ADDRESS < memory_len ) {
        int inst = memory[CURRENT_MEMORY_ADDRESS];                
        switch(inst) {
            // LOAD
            case 3:            
                if ( CURRENT_MEMORY_ADDRESS + 1 < memory_len ) {
                    accumulator = memory[ memory[CURRENT_MEMORY_ADDRESS + 1] ];
                    CURRENT_MEMORY_ADDRESS += 2;
                }else {
                    g_printerr(
                    "No operand found for instruction load at : memory[ %d ]\n",
                     CURRENT_MEMORY_ADDRESS + 1 );
                }
            break;
            // STORE
            case 7:
                if ( CURRENT_MEMORY_ADDRESS + 1 < memory_len ) {
                    memory[ memory[CURRENT_MEMORY_ADDRESS + 1] ] = accumulator;
                    CURRENT_MEMORY_ADDRESS += 2;
                }else { 
                    g_printerr(
                    "No operand found for instruction store at : memory[ %d ]\n",
                     CURRENT_MEMORY_ADDRESS + 1 );
                }
            break;
            default:
                g_printerr( "Not an instruction at memory address : %d \n",
                        CURRENT_MEMORY_ADDRESS );                
            break;
        }
    } else  {
        g_printerr("Memory limit reached\n");
    }
    char prefix[] = "ACCUM : ";
    char acm_str[100];
    snprintf(acm_str, 100, "%ld", accumulator); 
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(data, 
                    "accumulator")), strcat(prefix,  acm_str) );
}

void step(GtkWidget *widget, gpointer builder) {
    execute_current_instruction(builder);
    update_inst_pc(builder, memory[CURRENT_MEMORY_ADDRESS]); 
}

#endif // MAQUINA_H

