
#include "maquina.h"
#include <string.h>
#include <limits.h>
#include <assert.h>
#include "types.h"
#include "helper.h"
#include "stack.h"
#include "architecture.h"

const char * const enum_str[] = {

    [LOAD_DIRECT] = "LOAD  &",
    [LOAD_INDIRECT] = "LOAD  #",
    [LOAD_IMMEDIATE] = "LOAD  ",
    [STORE_DIRECT] = "STORE  &",
    [STORE_INDIRECT] = "STORE  #",
    [READ_DIRECT]  = "READ  &",
    [READ_INDIRECT]  = "READ  #",
    [STOP]  = "STOP",
    [ADD_DIRECT]  = "ADD  &",
    [ADD_INDIRECT]  = "ADD  #",
    [ADD_IMMEDIATE]  = "ADD  ",
    [BRZERO_DIRECT]  = "BRZERO  &",
    [BRZERO_INDIRECT]  = "BRZERO  #",
    [BRPOS_DIRECT]  = "BRPOS  &",
    [BRPOS_INDIRECT]  = "BRPOS  #",
    [BRNEG_DIRECT]  = "BRNEG  &",
    [BRNEG_INDIRECT]  = "BRNEG  #",
    [WRITE_DIRECT]  = "WRITE  &",
    [WRITE_INDIRECT]  = "WRITE  #",
    [MULT_DIRECT]  = "MULT  &",
    [MULT_INDIRECT]  = "MULT  #",
    [MULT_IMMEDIATE]  = "MULT  ",
    [DIV_DIRECT]  = "DIV  &",
    [DIV_INDIRECT]  = "DIV  #",
    [DIV_IMMEDIATE]  = "DIV  ",
    [SUB_DIRECT] = "SUB  &",
    [SUB_INDIRECT] = "SUB  #",
    [SUB_IMMEDIATE] = "SUB  ",
    [BR_DIRECT]  = "BR  &",
    [BR_INDIRECT]  = "BR  #",
    [BR_IMMEDIATE]  = "BR  ",
    [CALL_DIRECT]  = "CALL  &",
    [CALL_INDIRECT] = "CALL  #",
    [PUT_DIRECT]  = "PUT  &",
    [PUT_INDIRECT]  = "PUT  #",
    [RET]  = "RET",
    [UNKNOWN] = "????",

};

void update_inst_pc(GtkBuilder *builder, int inst) {

    char pc_str[10], inst_str[10], operand1[10];  
    char ri_str[10], sp_str[10];
    
    snprintf(pc_str, 10, "%d", program_counter);
    snprintf(inst_str, 10, "%d", inst);

    if ( program_counter < MEMORY_SIZE - 1 ) {
        snprintf(operand1, 10, "%d", memory[program_counter + 1]);
    } else {
        strcpy(operand1, "XXXX");
    }

    ri = inst;
    snprintf(ri_str, 10, "%d", ri);
    snprintf(sp_str, 10, "%d", stack_pointer);

    char *write_sp =
        ( char * ) malloc ( strlen("SP : ") + strlen(sp_str) );
    
    strcpy( write_sp, "SP : " );
    strcat( write_sp, sp_str  );

    char *write_ri =
        ( char * ) malloc ( strlen("RI : ") + strlen(ri_str) );

    strcpy( write_ri, "RI : " );
    strcat( write_ri, ri_str  );

    char *write_pc =
        ( char * ) malloc ( strlen("PC : ") + strlen(pc_str) );

    strcpy( write_pc , "PC : " );
    strcat( write_pc , pc_str );

    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,
                      "program_counter")), write_pc );

    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,
                      "instruction_register")), write_ri );

    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,
                      "stack_pointer")), write_sp );

    char *write_buff =
        ( char * ) malloc( strlen(enum_str[inst]) + strlen(operand1) + 1 );
   
    if (   inst != RET 
        && inst != STOP  
        && inst != UNKNOWN ) {

        strcpy ( write_buff, enum_str[inst] );
        strcat ( write_buff, operand1 );

        gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,
                           "CURRENT_MEMORY_VALUE")),
                           write_buff );

    }else { 
        switch (inst) {
            // STOP
            case STOP:
                gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,
                               "CURRENT_MEMORY_VALUE")),
                               enum_str[STOP] );
            break;
            // RET
            case RET: 
                gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,
                                "CURRENT_MEMORY_VALUE")),
                                enum_str[RET] );
            break;
            // UNKNOWN
            default: 
                gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,
                               "CURRENT_MEMORY_VALUE")),
                               enum_str[UNKNOWN] );
            break;
        } 
    }

    free(write_buff);
    free(write_ri);
    free(write_sp);
    free(write_pc);

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
            case LOAD_DIRECT:            
                if ( program_counter < MEMORY_SIZE - 1 ) {
                    int operand = memory[ program_counter + 1 ];
                    if ( operand < MEMORY_SIZE && operand >= 0 ) {
                        accumulator = memory[ operand ];
                        program_counter += 2;
                    } else {
                        snprintf(buffer,
                        sizeof(buffer),
                        "Invalid memory address for first"
                        "operand of instruction load at : %d \n",
                        operand);
                        append_text_to_text_view( textview, buffer );
                    }
                } else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "No operand found for instruction load at : memory[ %d ]\n",
                    program_counter + 1);
                    append_text_to_text_view( textview, buffer );
                }
            break;
            // STORE
            case STORE_DIRECT:
                if ( program_counter < MEMORY_SIZE - 1 ) {
                    int operand = memory [ program_counter + 1 ];
                    if ( operand < MEMORY_SIZE && operand >= 0 ){
                        memory[ operand ] = accumulator;
                        program_counter += 2;
                    } else {
                        snprintf(buffer,
                        sizeof(buffer),
                        "Invalid memory address for first"
                        "operand of instruction store at : %d \n",
                        operand);
                        append_text_to_text_view( textview, buffer );
                    }
                } else { 
                    snprintf(buffer,
                    sizeof(buffer),
                    "\nNo operand found for instruction store at : memory [ %d ]\n",
                    program_counter + 1);
                    append_text_to_text_view( textview, buffer );
                }
            break;
            // READ
            case READ_DIRECT: 
                if ( program_counter < MEMORY_SIZE - 1 ) {
                    int operand = memory [ program_counter + 1 ];
                    if ( operand < MEMORY_SIZE && operand >= 0 ) {
                        GtkWidget *container_window =
                        GTK_WIDGET(gtk_builder_get_object(builder, "CONTAINER_WINDOW"));
                        // This is blocking vvvv
                        word_t number = show_number_input_dialog(NULL,
                                container_window);   
                        memory[ operand ] = number; 
                        program_counter += 2;
                    }
                }
            break;
            // STOP
            case STOP:
                append_text_to_text_view( textview,
                        "\nProgram terminated with exit status success.\n");
            break;
            // ADD
            case ADD_DIRECT:
                if ( program_counter < MEMORY_SIZE - 1 ) {
                    int operand = memory[ program_counter + 1 ];
                    if ( operand < MEMORY_SIZE && operand >= 0 ) {
                        if (abs( accumulator + memory[ operand ] ) > 
                            WORD_MAX ) {
                            snprintf(buffer,
                            sizeof(buffer),
                            "\nInteger overflow occurred when performing "
                            "instruction ADD with memory location %d, " 
                            "PC : %d, SP : %d \n",
                            operand, program_counter, stack_pointer);
                            append_text_to_text_view(textview, buffer);
                        }
                        accumulator += memory[ operand ];
                        program_counter += 2;
                    } else {
                        snprintf(buffer,
                        sizeof(buffer),
                        "\nInvalid memory address for first "
                        "operand of instruction ADD at : %d\n",
                         operand );
                        append_text_to_text_view( textview, buffer );
                    }
                } else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "\nNo operand found for instruction add at : memory[ %d ]\n",
                    program_counter + 1);
                    append_text_to_text_view( textview, buffer );
                }
            break;
            // BRZERO
            case BRZERO_DIRECT:
                if ( program_counter < MEMORY_SIZE - 1 ) {
                    int operand = memory[ program_counter + 1 ];
                    if ( operand < MEMORY_SIZE && operand >= 0 ) {
                        if ( !accumulator ) {
                            program_counter = memory[ operand ]; 
                        } else { 
                            program_counter += 2;
                        }
                    } else {
                        snprintf(buffer,
                        sizeof(buffer),
                        "\nInvalid memory address for first "
                        "operand of instruction BRZERO at : %d\n",
                        operand );
                        append_text_to_text_view( textview, buffer );
                    }
                } else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "\nNo operand found for instruction BRZERO at : memory[ %d ]\n",
                    program_counter + 1);
                    append_text_to_text_view( textview, buffer );
                }
            break;
            // BRPOS
            case BRPOS_DIRECT:
                if ( program_counter < MEMORY_SIZE - 1 ) {
                    int operand = memory[ program_counter + 1 ];
                    if ( operand < MEMORY_SIZE && operand >= 0 ) {
                        if ( accumulator > 0 ) {
                            program_counter = memory[ operand ]; 
                        } else { 
                            program_counter += 2;
                        }
                    } else {
                        snprintf(buffer,
                        sizeof(buffer),
                        "\nInvalid memory address for first "
                        "operand of instruction BRPOS at : %d\n",
                        operand );
                        append_text_to_text_view( textview, buffer );
                    }
                } else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "\nNo operand found for instruction BRPOS at : memory[ %d ]\n",
                    program_counter + 1);
                    append_text_to_text_view( textview, buffer );
                }
            break;
            // BRNEG
            case BRNEG_DIRECT:
                if ( program_counter < MEMORY_SIZE - 1 ) {
                    int operand = memory[ program_counter + 1 ];
                    if ( operand < MEMORY_SIZE && operand >= 0 ) {
                        if ( accumulator < 0 ) {
                            program_counter = memory[ operand ]; 
                        } else { 
                            program_counter += 2;
                        }
                    } else {
                        snprintf(buffer,
                        sizeof(buffer),
                        "\nInvalid memory address for first "
                        "operand of instruction BRNEG at : %d\n",
                        operand );
                        append_text_to_text_view( textview, buffer );
                    }
                } else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "\nNo operand found for instruction BRNEG at : memory[ %d ]\n",
                    program_counter + 1);
                    append_text_to_text_view( textview, buffer );
                }
            break;
            // WRITE
            case WRITE_DIRECT:
                if ( program_counter < MEMORY_SIZE - 1 ) {
                    word_t operand = memory[ program_counter + 1 ];
                    if ( operand < MEMORY_SIZE && operand >= 0 ) {
                     
                        // Coloca a string na posição de memoria do
                        // operando de WRITE no stdout ou console
                        // com sistema little endian ( little endianness )
                        // ou seja o primeiro character é os 8 bits menos
                        // significativos o segundo os 8 mais significativos.
                        
                        char lu[3];
                        word_t words = 1;
                        word_t iterator = memory[ operand ];
                        while (iterator && operand + words < MEMORY_SIZE ) {
                            static_assert(sizeof(char) == 1, "The char is not 1 byte.");
                            word_t lower = WORD_LOWER_BYTE(iterator);
                            word_t upper = WORD_UPPER_BYTE(iterator);
                            char lower_c = ( char ) ( lower );
                            char upper_c = ( char ) ( upper );
                            lu[0] = lower_c;
                            lu[1] = upper_c;
                            lu[2] = '\0';
                            append_text_to_text_view( textview, lu);
                            iterator = memory[ operand + words ];
                            words++;
                        }

                        program_counter += 2;

                    } else {
                        snprintf(buffer,
                        sizeof(buffer),
                        "\nInvalid memory address for first "
                        "operand of instruction WRITE at : %d\n",
                        operand );
                        append_text_to_text_view( textview, buffer );
                    }
                } else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "\nNo operand found for instruction WRITE at : memory[ %d ]\n",
                    program_counter + 1);
                    append_text_to_text_view( textview, buffer );
                }
            break;
            // MULT
            case MULT_DIRECT:
                if ( program_counter < MEMORY_SIZE - 1 ) {
                    int operand = memory[program_counter + 1];
                    if ( operand < MEMORY_SIZE && operand >= 0 ) {
                        accumulator *= memory[operand];
                        program_counter += 2;
                    }
                }
            break;
            // DIV
            case DIV_DIRECT:
                if( program_counter < MEMORY_SIZE - 1 ) {
                    int operand = memory[program_counter + 1];
                    if ( operand < MEMORY_SIZE && operand >= 0 ) {
                        accumulator /= memory[operand];
                        program_counter += 2;
                    }
                }
            break;
            // BR
            case BR_DIRECT:
                if ( program_counter < MEMORY_SIZE - 1 ) {
                    int operand = memory[program_counter + 1];
                    if ( operand < MEMORY_SIZE && operand >= 0) {
                        program_counter = memory[operand];
                    }
                }
            break;
            // PUT
            case PUT_DIRECT:
                if ( program_counter < MEMORY_SIZE - 1 ) {
                    int operand = memory[program_counter + 1];
                    if ( operand < MEMORY_SIZE && operand >= 0) {
                        char number[256];
                        snprintf(number, 256, "%d", memory[ operand ]);
                        append_text_to_text_view(textview, number); 
                        program_counter+=2;
                    }
                }
            break;
            // CALL
            case CALL_DIRECT:
                if ( program_counter < MEMORY_SIZE - 1 ) {
                    int operand = memory[program_counter + 1];
                    if ( operand < MEMORY_SIZE && operand >= 0) {
                        bool status = push(program_counter + 2);
                        if (!status) {
                            snprintf(buffer,
                            sizeof(buffer),
                            "\nStack overflow occurred  PC : %d "
                            "SP : %d \n",
                            program_counter, stack_pointer );
                            append_text_to_text_view( textview, buffer );
                        }
                        program_counter = memory[ operand ]; 
                    }
                }
            break;
            // RET
            case RET:
                if ( program_counter < MEMORY_SIZE - 1 ) {
                    int operand = memory[program_counter + 1];
                    if ( operand < MEMORY_SIZE && operand >= 0) {
                        program_counter = pop();
                    }
                }
            break;
            case SUB_DIRECT:
                if ( program_counter < MEMORY_SIZE - 1 ) {
                    int operand = memory[program_counter + 1];
                    if ( operand < MEMORY_SIZE && operand >= 0) {
                        accumulator -= memory[ operand ];
                        program_counter += 2;
                    }
                }
            break;
            // UNKNOWN 
            default:
                snprintf(buffer,
                sizeof(buffer),
                "\nNot an instruction at memory address : %d \n",
                program_counter);
                append_text_to_text_view( textview, buffer );
            break;
        }
    } else  {
        textview =
        GTK_TEXT_VIEW(gtk_builder_get_object(builder, "console"));
        append_text_to_text_view( textview, "\nMemory limit reached. \n" );
    }
    char prefix[] = "ACCUM : ";
    char acm_str[100];
    snprintf(acm_str, 100, "%d", accumulator); 
    gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(data, 
                    "accumulator")), strcat(prefix,  acm_str) );

}

void update_memory_tree(void* data) {

    user_data_t *d = data;
    GtkListStore *store = GTK_LIST_STORE(d->store);
    GtkTreeView *treeview = GTK_TREE_VIEW(d->treeview);
    GtkTreeIter iter;

    GtkTreePath *path = NULL;
    GtkTreeViewColumn *col = NULL;
    gtk_tree_view_get_cursor(treeview, &path, &col);

    GtkScrolledWindow *sw = GTK_SCROLLED_WINDOW(gtk_builder_get_object(d->builder,
                            "memory_scroll" ));

    gdouble vscroll, hscroll; 
    get_scroll_position(sw, &vscroll, &hscroll);

    gtk_list_store_clear(store);

    for (int i = 0; i < MEMORY_SIZE; i++) {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, i, 1, memory[i], -1);
    }

    if ( path != NULL ) {
        gtk_tree_view_set_cursor(treeview, path, col, FALSE);
        gtk_tree_path_free(path);
    }

    set_scroll_position(sw, vscroll, hscroll);
}

void step(GtkWidget *widget, gpointer data) {
    user_data_t *user_data_t = data;  
    execute_current_instruction(user_data_t->builder);
    update_inst_pc(user_data_t->builder, memory[program_counter]); 
    update_memory_tree(user_data_t);
}

void run(GtkWidget *widget, gpointer data) {

    user_data_t *ud = data;
    GtkTextView *textview = GTK_TEXT_VIEW(gtk_builder_get_object(ud->builder,
                "console"));

    int inst;
    do {
        if ( program_counter < MEMORY_SIZE ) {
            inst = memory[ program_counter ];
            step( (void *)0, ud);
        } else { 
           append_text_to_text_view(textview,
                   "SEGFAULT: PC outside of memory range"); 
           break;
        }
    }while ( inst != STOP && ( mop != MOP_NOT_DEFINED ) );

}

