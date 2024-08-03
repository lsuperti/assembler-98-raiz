#ifndef MAQUINA_H
#define MAQUINA_H

#include <stdio.h>
#include <gtk/gtk.h>
#include "types.h"
#include "helper.h"
#include <limits.h>
#define MEMORY_SIZE 1024 

// 16kB of memory.
int16_t memory[MEMORY_SIZE];

// Registers
// Stack size = 4kB
// Data region size = 4kB
// Instruction region size = 8kB
int16_t accumulator;
int16_t stack_pointer = 255;
int16_t program_counter = 256;
int8_t  mop = 0;
int16_t ri = 0;
int16_t data_reg = 767;

void update_inst_pc( GtkBuilder *builder, int inst) {

    char pc[100], inst_str[100], operand1[100];  
    snprintf(pc, 100, "%d", program_counter);
    snprintf(inst_str, 100, "%d", inst);
    if ( program_counter + 1 < MEMORY_SIZE ) {
        snprintf(operand1, 100, "%d", memory[program_counter + 1]);
    } else {
        strcpy(operand1, "XXXX");
    }

    char prefix[100] = "PC : ";
    char store[100] = "STORE  ";
    char load[100] = "LOAD  ";
    char read[100] = "READ  ";
    char add[100] = "ADD  ";
    char brzero[100] = "BRZERO  ";
    char brpos[100] = "BRPOS  ";
    char brneg[100] = "BRNEG  ";
    char write[100] = "WRITE  ";
    char mult[100] = "MULT ";
    char div[100] = "DIV ";
    char br[100] = "BR ";
    char unknown[100] = "????  ";

    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,
                      "program_counter")), strcat(prefix, pc) );

    // Set instruction register to the current opcode.
    ri = inst;

    switch (inst) {
        // LOAD
        case 3: 
            gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,
                           "CURRENT_MEMORY_VALUE")),
                           strcat(load, operand1));
        break;
        // STORE
        case 7:
            gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,
                           "CURRENT_MEMORY_VALUE")),
                           strcat(store, operand1));
        break;
        // READ
        case 12:
            gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,
                           "CURRENT_MEMORY_VALUE")),
                           strcat(read, operand1));
        break;
        // STOP
        case 11:
            gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,
                           "CURRENT_MEMORY_VALUE")), "STOP");
        break;
        // ADD
        case 2:
            gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,
                           "CURRENT_MEMORY_VALUE")),
                           strcat(add, operand1));
        break;
        // BRZERO
        case 4:
            gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,
                           "CURRENT_MEMORY_VALUE")),
                           strcat(brzero, operand1));
        break;
        // BRPOS
        case 1:
            gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,
                           "CURRENT_MEMORY_VALUE")),
                           strcat(brpos, operand1));
        break;
        // BRNEG
        case 5:
            gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,
                           "CURRENT_MEMORY_VALUE")),
                           strcat(brneg, operand1));
        break;
        // WRITE
        case 8:
            gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,
                           "CURRENT_MEMORY_VALUE")),
                           strcat(write, operand1));
        break;
        // MULT
        case 14:
            gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,
                            "CURRENT_MEMORY_VALUE")),
                            strcat(mult, operand1));
        break;
        // DIV
        case 10:
            gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,
                            "CURRENT_MEMORY_VALUE")),
                            strcat(div, operand1));
        break;
        // BR
        case 00:
            gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,
                            "CURRENT_MEMORY_VALUE")),
                            strcat(br, operand1));
        break;
        // UNKNOWN
        default: 
            gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,
                           "CURRENT_MEMORY_VALUE")),
                           unknown);
        break;
    }
}

void reset(GtkWidget *widget, gpointer builder) {
    program_counter = 256;
    update_inst_pc(builder, memory[program_counter]);
}

void execute_current_instruction(void* data) {
    
    GtkBuilder *builder = data;
    GtkTextView *textview =  
                GTK_TEXT_VIEW(gtk_builder_get_object(builder, "console"));
    char buffer[512];

    if ( program_counter < MEMORY_SIZE ) {
        int inst = memory[program_counter];      
        switch(inst) {
            // LOAD
            case 3:            
                if ( program_counter + 1 < MEMORY_SIZE ) {
                    int operand = memory[ program_counter + 1 ];
                    if ( operand < MEMORY_SIZE && operand >= 0 ) {
                        accumulator = memory[ operand ];
                        program_counter += 2;
                    }else {
                        g_printerr( 
                        "Invalid memory address for first "
                        "operand of instruction load at : %d \n",
                         operand );
                    }
                }else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "No operand found for instruction load at : memory[ %d ]\n",
                    program_counter + 1);

                    append_text_to_text_view( textview, buffer );
                }
            break;
            // STORE
            case 7:
                if ( program_counter + 1 < MEMORY_SIZE ) {
                    int operand = memory [ program_counter + 1 ];
                    if ( operand < MEMORY_SIZE && operand >= 0 ){
                        memory[ operand ] = accumulator;
                        program_counter += 2;
                    }else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "Invalid memory address for first"
                    "operand of instruction store at : %d \n",
                    operand);

                    append_text_to_text_view( textview, buffer );
                    }
                }else { 
                    g_printerr(
                    "No operand found for instruction store at : memory[ %d ]\n",
                     program_counter + 1 );
                }
            break;
            // READ
            case 12: 
                  if ( program_counter + 1 < MEMORY_SIZE ) {
                     int operand = memory [ program_counter + 1 ];
                     if ( operand < MEMORY_SIZE && operand >= 0 ){
                        GtkWidget *window =
                        GTK_WIDGET(gtk_builder_get_object(builder, "window"));
                        // This is blocking vvvv
                        int16_t number = show_number_input_dialog(NULL, window);   
                        memory[ operand ] = number; 
                        program_counter += 2;
                     }
                  }
            break;
            // STOP
            case 11:
                append_text_to_text_view( textview,
                        "Program terminated with exit status success.\n");
            break;
            // ADD
            case 2:
                if ( program_counter + 1 < MEMORY_SIZE ) {
                    int operand = memory[ program_counter + 1 ];
                    if ( operand < MEMORY_SIZE && operand >= 0 ) {
                        if (abs( accumulator + memory[ operand ] ) > 
                            UINT16_MAX ) {
                            snprintf(buffer,
                            sizeof(buffer),
                            "Integer overflow occurred when performing "
                            "instruction ADD with memory location %d, " 
                            "PC : %d, SP : %d \n",
                            operand, program_counter, stack_pointer);
                            
                            append_text_to_text_view(textview, buffer);
                        }
                        accumulator += memory[ operand ];
                        program_counter += 2;
                    }else {
                        snprintf(buffer,
                        sizeof(buffer),
                        "Invalid memory address for first "
                        "operand of instruction ADD at : %d\n",
                         operand );

                        append_text_to_text_view( textview, buffer );
                    }
                }else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "No operand found for instruction add at : memory[ %d ]\n",
                    program_counter + 1);

                    append_text_to_text_view( textview, buffer );
                }
            break;
            // BRZERO
            case 4:
                if ( program_counter + 1 < MEMORY_SIZE ) {
                    int operand = memory[ program_counter + 1 ];
                    if ( operand < MEMORY_SIZE && operand >= 0 ) {
                        if ( !accumulator ) {
                           program_counter = memory[ operand ]; 
                        } else { 
                           program_counter += 2;
                        }
                    }else {
                        snprintf(buffer,
                        sizeof(buffer),
                        "Invalid memory address for first "
                        "operand of instruction BRZERO at : %d\n",
                         operand );

                        append_text_to_text_view( textview, buffer );
                    }
                }else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "No operand found for instruction BRZERO at : memory[ %d ]\n",
                    program_counter + 1);

                    append_text_to_text_view( textview, buffer );
                }
            break;
            // BRPOS
            case 1:
                if ( program_counter + 1 < MEMORY_SIZE ) {
                    int operand = memory[ program_counter + 1 ];
                    if ( operand < MEMORY_SIZE && operand >= 0 ) {
                        if ( accumulator > 0 ) {
                           program_counter = memory[ operand ]; 
                        } else { 
                           program_counter += 2;
                        }
                    }else {
                        snprintf(buffer,
                        sizeof(buffer),
                        "Invalid memory address for first "
                        "operand of instruction BRPOS at : %d\n",
                         operand );

                        append_text_to_text_view( textview, buffer );
                    }
                }else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "No operand found for instruction BRPOS at : memory[ %d ]\n",
                    program_counter + 1);

                    append_text_to_text_view( textview, buffer );
                }
            break;
            // BRNEG
            case 5:
                if ( program_counter + 1 < MEMORY_SIZE ) {
                    int operand = memory[ program_counter + 1 ];
                    if ( operand < MEMORY_SIZE && operand >= 0 ) {
                        if ( accumulator < 0 ) {
                           program_counter = memory[ operand ]; 
                        } else { 
                           program_counter += 2;
                        }
                    }else {
                        snprintf(buffer,
                        sizeof(buffer),
                        "Invalid memory address for first "
                        "operand of instruction BRNEG at : %d\n",
                         operand );

                        append_text_to_text_view( textview, buffer );
                    }
                }else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "No operand found for instruction BRNEG at : memory[ %d ]\n",
                    program_counter + 1);

                    append_text_to_text_view( textview, buffer );
                }
            break;
            // WRITE
            case 8:
                if ( program_counter + 1 < MEMORY_SIZE ) {
                    int16_t operand = memory[ program_counter + 1 ];
                    if ( operand < MEMORY_SIZE && operand >= 0 ) {
                     
                         // Gera a string na posição de memoria do
                         // operando de WRITE, com sistema little endian
                         // ou seja o primeiro character é os 8 bits menos
                         // significativos o segundo os 8 mais significativos.
                         
                         char lu[2];
                         int16_t bytes = 1;
                         int16_t iterator = memory[ operand ];
                         while (iterator) {
                            uint16_t lower = iterator; 
                            uint16_t upper = iterator;
                            lower = ( lower << 8 ) >> 8;
                            upper = ( upper >> 8 );
                            iterator = memory[ operand + bytes ];
                            bytes++;
                            char lower_c = ( char ) lower;
                            char upper_c = ( char ) upper;
                            lu[0] = lower_c;
                            lu[1] = upper_c;
                            append_text_to_text_view( textview, lu);
                         }

                         program_counter += 2;

                    }else {
                        snprintf(buffer,
                        sizeof(buffer),
                        "Invalid memory address for first "
                        "operand of instruction WRITE at : %d\n",
                         operand );

                        append_text_to_text_view( textview, buffer );
                    }
                }else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "No operand found for instruction WRITE at : memory[ %d ]\n",
                    program_counter + 1);

                    append_text_to_text_view( textview, buffer );
                }
            break;
            // MULT
            case 14:
                if ( program_counter + 1 < MEMORY_SIZE ) {
                    int operand = memory[program_counter + 1];
                    if ( operand < MEMORY_SIZE && operand >= 0 ) {
                        accumulator *= memory[operand];
                        program_counter += 2;
                    }
                }
            break;
            // DIV
            case 10:
                if( program_counter + 1 < MEMORY_SIZE ) {
                    int operand = memory[program_counter + 1];
                    if ( operand < MEMORY_SIZE && operand >= 0 ) {
                        accumulator /= memory[operand];
                        program_counter += 2;
                    }
                }
            break;
            // BR
            case 00:
                if ( program_counter + 1 < MEMORY_SIZE ) {
                    int operand = memory[program_counter + 1];
                    if ( operand < MEMORY_SIZE && operand >= 0) {
                        program_counter = memory[operand];
                    }
                }
            break;
            // UNKNOWN 
            default:
                snprintf(buffer,
                sizeof(buffer),
                "Not an instruction at memory address : %d \n",
                program_counter);

                append_text_to_text_view( textview, buffer );
            break;
        }
    } else  {
        textview =
        GTK_TEXT_VIEW(gtk_builder_get_object(builder, "console"));
        append_text_to_text_view( textview, "Memory limit reached. \n" );
    }
    char prefix[] = "ACCUM : ";
    char acm_str[100];
    snprintf(acm_str, 100, "%d", accumulator); 
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(data, 
                    "accumulator")), strcat(prefix,  acm_str) );

            
}

void update_memory_tree(void* user_data) {
    GtkListStore *store = GTK_LIST_STORE(user_data);
    GtkTreeIter iter;
    gtk_list_store_clear(store);
    for (int i = 0; i < MEMORY_SIZE; i++) {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, i, 1, memory[i], -1);
    }
}

void step(GtkWidget *widget, gpointer data) {
    user_data *user_data = data;  
    execute_current_instruction(user_data->builder);
    update_inst_pc(user_data->builder, memory[program_counter]); 
    update_memory_tree(user_data->store);
}

#endif // MAQUINA_H

