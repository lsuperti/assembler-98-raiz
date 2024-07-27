#ifndef MAQUINA_H
#define MAQUINA_H

#include <stdio.h>
#include <gtk/gtk.h>
#include "types.h"
#define MEMORY_SIZE 512

int CURRENT_MEMORY_ADDRESS = 0;
int memory[MEMORY_SIZE] = { 3, 45, 7, 33, 0 }; 
long int accumulator;

void update_inst_pc( GtkBuilder *builder, int inst) {
    char pc[100], inst_str[100], operand1[100];  
    snprintf(pc, 100, "%d", CURRENT_MEMORY_ADDRESS);
    snprintf(inst_str, 100, "%d", inst);
    if ( CURRENT_MEMORY_ADDRESS + 1 < MEMORY_SIZE ) {
        snprintf(operand1, 100, "%d", memory[CURRENT_MEMORY_ADDRESS + 1]);
    } else {
        strcpy(operand1, "XXXX");
    }
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
                           unknown);
        break;
    }
}

void reset(GtkWidget *widget, gpointer builder) {
    CURRENT_MEMORY_ADDRESS = 0;
    update_inst_pc(builder, memory[CURRENT_MEMORY_ADDRESS]);
}

void execute_current_instruction(void* data) {
    if ( CURRENT_MEMORY_ADDRESS < MEMORY_SIZE ) {
        int inst = memory[CURRENT_MEMORY_ADDRESS];      
        switch(inst) {
            // LOAD
            case 3:            
                if ( CURRENT_MEMORY_ADDRESS + 1 < MEMORY_SIZE ) {
                    int operand = memory[ CURRENT_MEMORY_ADDRESS + 1 ];
                    if ( operand < MEMORY_SIZE && operand >= 0 ) {
                        accumulator = memory[ operand ];
                        CURRENT_MEMORY_ADDRESS += 2;
                    }else {
                        g_printerr( 
                        "Invalid memory address for first "
                        "operand of instruction load at : %d \n",
                         operand );
                    }
                }else {
                    g_printerr(
                    "No operand found for instruction load at : memory[ %d ]\n",
                     CURRENT_MEMORY_ADDRESS + 1 );
                }
            break;
            // STORE
            case 7:
                if ( CURRENT_MEMORY_ADDRESS + 1 < MEMORY_SIZE ) {
                    int operand = memory [ CURRENT_MEMORY_ADDRESS + 1 ];
                    if ( operand < MEMORY_SIZE && operand >= 0 ){
                        memory[ operand ] = accumulator;
                        CURRENT_MEMORY_ADDRESS += 2;
                    }else {
                        g_printerr( 
                        "Invalid memory address for first "
                        "operand of instruction store at : %d \n",
                         operand );
                    }
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

void update_memory_tree(void* user_data) {
    GtkListStore *store = GTK_LIST_STORE(user_data);
    GtkTreeIter iter;

    gtk_list_store_clear(store);

    for (int i = 0; i < 512; i++) {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, i, 1, memory[i], -1);
    }
}

void step(GtkWidget *widget, gpointer data) {
    user_data *user_data = data;  
    execute_current_instruction(user_data->builder);
    update_inst_pc(user_data->builder, memory[CURRENT_MEMORY_ADDRESS]); 
    update_memory_tree(user_data->store);
}

#endif // MAQUINA_H

