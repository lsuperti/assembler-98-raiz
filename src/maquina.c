
#include "maquina.h"
#include <string.h>
#include <limits.h>
#include <assert.h>
#include "types.h"
#include "global.h"
#include "montador.h"
#include "helper.h"
#include "stack.h"
#include "macro_processor.h"
#include "architecture.h"

bool running = false;
#define ENUM_STR_SIZE (sizeof(enum_str) / sizeof(enum_str[0]))

const char * const tok_colors[] = {

    [TOK_PUT]             = "red",
    [TOK_SPACE]           = "purple",
    [TOK_WRITE]           = "red",
    [TOK_SUB]             = "red",
    [TOK_STORE]           = "red",
    [TOK_READ]            = "red",
    [TOK_MULT]            = "red",
    [TOK_DIVIDE]          = "red",
    [TOK_CALL]            = "red",
    [TOK_BRZERO]          = "red",
    [TOK_BRPOS]           = "red",
    [TOK_BRNEG]           = "red",
    [TOK_BR]              = "red",
    [TOK_ADD]             = "red",
    [TOK_LOAD]            = "red",
    [TOK_EXTERN]          = "red",
    [TOK_GLOBAL]          = "red",
    [TOK_ASCIIZ]          = "purple",
    [TOK_WORD]            = "purple",
    [TOK_STRING]          = "green",
    [TOK_SECTION_NAME]    = "purple",
    [TOK_LABEL]           = "gray",
    [TOK_STOP]            = "red",
    [TOK_RET]             = "red",
    [TOK_COPY]            = "red",
    [TOK_STACK]           = "red",
    [TOK_LITERAL_HEX]     = "gray",
    [TOK_LITERAL]         = "gray",
    [TOK_ADDRESSING]      = "orange",
    [TOK_SECTION]         = "red",
    [TOK_IDENTIFIER]      = "blue",
    [TOK_MACRO_START]     = "orange",
    [TOK_MACRO_END]       = "orange",
    [TOK_LOCAL_LABEL]     = "gray",
    [TOK_LOCAL_IDENTIFIER]= "blue",
    [TOK_NEWLINE]         = NULL,
    [TOK_PARAM_DELIMITER] = "orange",

};

const char * const enum_str[] = {

    [LOAD_DIRECT] = "LOAD  &",
    [LOAD_INDIRECT] = "LOAD  #",
    [LOAD_IMMEDIATE] = "LOAD  ",
    [STORE_DIRECT] = "STORE  &",
    [STORE_INDIRECT] = "STORE  #",
    [READ_DIRECT]  = "READ  &",
    [READ_INDIRECT]  = "READ  #",
    [COPY_DD] = "COPY ",
    [COPY_DI] = "COPY ",
    [COPY_DIm] = "COPY ",
    [COPY_ID] = "COPY ",
    [COPY_II] = "COPY ",
    [COPY_IIm] = "COPY ",
    [STOP]  = "STOP",
    [ADD_DIRECT]  = "ADD  &",
    [ADD_INDIRECT]  = "ADD  #",
    [ADD_IMMEDIATE]  = "ADD  ",
    [BRZERO_DIRECT]  = "BRZERO  &",
    [BRZERO_IMMEDIATE]  = "BRZERO ",
    [BRZERO_INDIRECT]  = "BRZERO  #",
    [BRPOS_DIRECT]  = "BRPOS  &",
    [BRPOS_IMMEDIATE]  = "BRPOS ",
    [BRPOS_INDIRECT]  = "BRPOS  #",
    [BRNEG_DIRECT]  = "BRNEG  &",
    [BRNEG_IMMEDIATE]  = "BRNEG ",
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
    [BR_IMMEDIATE]  = "BR ",
    [CALL_DIRECT]  = "CALL  &",
    [CALL_INDIRECT] = "CALL  #",
    [CALL_IMMEDIATE] = "CALL ",
    [PUT_DIRECT]  = "PUT  &",
    [PUT_INDIRECT]  = "PUT  #",
    [RET]  = "RET",
    [UNKNOWN] = "????",

};

void update_inst_pc(GtkBuilder *builder, int inst) {

    char pc_str[10], inst_str[10], operand1[10];  
    char ri_str[10], sp_str[10], operand2[10];
    
    snprintf(pc_str, 10, "%d", program_counter);
    snprintf(inst_str, 10, "%d", inst);

    if ( program_counter < MEMORY_SIZE - 1 ) {
        snprintf(operand1, 10, "%d", memory[program_counter + 1]);
    } else {
        strcpy(operand1, "XXXX");
    }

    if ( program_counter < MEMORY_SIZE - 2 ) {
        snprintf(operand2, 10, "%d", memory[program_counter + 2]);
    } else {
        strcpy(operand2, "XXXX");
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
    char *write_buff;
    
    if ( inst >= 0 && inst < NUMBER_OF_INSTRUCTIONS){
        assert( enum_str[inst] != NULL );
        write_buff =
            ( char * ) malloc( strlen(enum_str[inst]) +
                    strlen(operand1) + strlen(operand2) + 4 );
    } 

    if (  inst != RET 
        &&inst != STOP  
        &&inst != UNKNOWN
        &&inst != COPY_DD
        &&inst != COPY_DI
        &&inst != COPY_DIm
        &&inst != COPY_ID 
        &&inst != COPY_II 
        &&inst != COPY_IIm
        && ( inst >= 0 && inst < NUMBER_OF_INSTRUCTIONS) ) {

        assert( enum_str[inst] != NULL );
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

        // COPY
        if ( is_copy(inst) ) {

            char _pr1[2], _pr2[3]; 

            switch(inst) {

                case COPY_DD:
                    strcpy( _pr1, "&");
                    strcpy( _pr2, " &");
                break;
                case COPY_DI:
                    strcpy( _pr1, "&");
                    strcpy( _pr2, " #");
                break;
                case COPY_DIm:
                    strcpy( _pr1, "&");
                    strcpy( _pr2, " ");
                break;
                case COPY_ID:
                    strcpy( _pr1, "#");
                    strcpy( _pr2, " &");
                break;
                case COPY_II:
                    strcpy( _pr1, "#");
                    strcpy( _pr2, " #");
                break;
                case COPY_IIm:
                    strcpy( _pr1, "#");
                    strcpy( _pr2, " ");
                break;

            }

            strcpy ( write_buff, enum_str[inst] );
            strcat ( write_buff, _pr1 );
            strcat ( write_buff, operand1 );
            strcat ( write_buff, _pr2 );
            strcat ( write_buff, operand2 );
            gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,
                            "CURRENT_MEMORY_VALUE")), write_buff);

        }

    }

    if ( inst >= 0 && inst < NUMBER_OF_INSTRUCTIONS){
        assert( enum_str[inst] != NULL );
        free(write_buff);
    }

    free(write_ri);
    free(write_sp);
    free(write_pc);

}

void reset(GtkWidget *widget, gpointer data) {
    user_data_t *user_data_t = data;
    program_counter = 256;
    update_inst_pc(user_data_t->builder, memory[program_counter]);
    update_memory_tree(user_data_t);
}

void execute_current_instruction(void* data) {
    
    GtkBuilder *builder = data;
    GtkTextView *textview =  
                GTK_TEXT_VIEW(gtk_builder_get_object(builder, "console"));
    GtkTextView *textview2 =  
                GTK_TEXT_VIEW(gtk_builder_get_object(builder, "console1"));

    char buffer[512];

    if ( program_counter < MEMORY_SIZE ) {
        int inst = memory[program_counter];      
        switch(inst) {
            // LOAD
            case LOAD_INDIRECT:
                if ( program_counter < MEMORY_SIZE - 1 ) {
                    int operand = memory[ program_counter + 1 ];
                    if ( operand < MEMORY_SIZE && operand >= 0 ) {
                        int m_op = memory [ operand ];
                        if ( m_op < MEMORY_SIZE && m_op >= 0 ) {
                           accumulator = memory [ m_op ]; 
                           program_counter += 2;
                        }else {
                            snprintf(buffer,
                            sizeof(buffer),
                            "Invalid memory address %d",
                            m_op);
                            append_text_to_text_view( textview, buffer );
                            append_text_to_text_view( textview2, buffer );
                        }
                    }else {
                        snprintf(buffer,
                        sizeof(buffer),
                        "Invalid memory address %d",
                        operand);
                        append_text_to_text_view( textview, buffer );
                        append_text_to_text_view( textview2, buffer );
                    }
                } else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "No operand found for instruction load at : memory[ %d ]\n",
                    program_counter + 1);
                    append_text_to_text_view( textview, buffer );
                    append_text_to_text_view( textview2, buffer );
                }
            case LOAD_IMMEDIATE:
                if ( program_counter < MEMORY_SIZE - 1 ) {
                    int operand = memory [ program_counter + 1 ];
                    accumulator = operand;
                    program_counter += 2;
                }else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "No operand found for instruction load at : memory[ %d ]\n",
                    program_counter + 1);
                    append_text_to_text_view( textview, buffer );
                    append_text_to_text_view( textview2, buffer );
                }
            break;
            case LOAD_DIRECT:            
                if ( program_counter < MEMORY_SIZE - 1 ) {
                    int operand = memory[ program_counter + 1 ];
                    if ( operand < MEMORY_SIZE && operand >= 0 ) {
                        accumulator = memory [ operand ];
                        program_counter += 2;
                    }else {
                            snprintf(buffer,
                            sizeof(buffer),
                            "Invalid memory address %d",
                            operand);
                            append_text_to_text_view( textview, buffer );
                            append_text_to_text_view( textview2, buffer );
                    }
                } else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "No operand found for instruction load at : memory[ %d ]\n",
                    program_counter + 1);
                    append_text_to_text_view( textview, buffer );
                    append_text_to_text_view( textview2, buffer );
                }
            break;
            // STORE
            case STORE_DIRECT:
                if ( program_counter < MEMORY_SIZE - 1 ){
                    int operand = memory [ program_counter + 1 ];
                    if ( operand < MEMORY_SIZE && operand >= 0 ) {
                        memory[ operand ] = accumulator;
                        program_counter += 2;
                    }else {
                        snprintf(buffer,
                        sizeof(buffer),
                        "Invalid memory address %d",
                        program_counter + 1);
                        append_text_to_text_view( textview, buffer );
                        append_text_to_text_view( textview2, buffer );
                    }
                }else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "Invalid memory address %d",
                    program_counter + 1);
                    append_text_to_text_view( textview, buffer );
                    append_text_to_text_view( textview2, buffer );
                }
            break;
            case STORE_INDIRECT:
                if ( program_counter < MEMORY_SIZE - 1 ) {
                    int operand = memory [ program_counter + 1 ];
                    if ( operand < MEMORY_SIZE && operand >= 0 ){
                        int m_op = memory[ operand ];
                        if ( m_op < MEMORY_SIZE && m_op >= 0 ){
                            memory [ m_op ] = accumulator; 
                            program_counter += 2;
                        } else {
                            snprintf(buffer,
                            sizeof(buffer),
                            "Invalid memory address %d",
                            m_op);
                            append_text_to_text_view( textview, buffer );
                            append_text_to_text_view( textview2, buffer );
                        }
                    } else {
                        snprintf(buffer,
                        sizeof(buffer),
                        "Invalid memory address for first"
                        "operand of instruction store at : %d \n",
                        operand);
                        append_text_to_text_view( textview, buffer );
                        append_text_to_text_view( textview2, buffer );
                    }
                } else { 
                    snprintf(buffer,
                    sizeof(buffer),
                    "\nNo operand found for instruction store at : memory [ %d ]\n",
                    program_counter + 1);
                    append_text_to_text_view( textview, buffer );
                    append_text_to_text_view( textview2, buffer );
                }
            break;
            // READ
            case READ_DIRECT: 
                if ( program_counter < MEMORY_SIZE - 1 ){
                    int operand = memory[ program_counter + 1 ];
                    if ( operand < MEMORY_SIZE && operand >= 0 ) {

                        GtkWidget *container_window =
                        GTK_WIDGET(gtk_builder_get_object(builder,
                                    "CONTAINER_WINDOW"));

                        word_t number = show_number_input_dialog(NULL,
                                container_window);   
                        memory[ operand ] = number;
                        program_counter += 2;
                    }else {
                            snprintf(buffer,
                            sizeof(buffer),
                            "Invalid memory address %d",
                            operand);
                            append_text_to_text_view( textview, buffer );
                            append_text_to_text_view( textview2, buffer );
                    }
                }else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "Invalid memory address %d",
                    program_counter + 1);
                    append_text_to_text_view( textview, buffer );
                    append_text_to_text_view( textview2, buffer );
                }
            break;
            case READ_INDIRECT:
                if ( program_counter < MEMORY_SIZE - 1 ) {
                    int operand = memory [ program_counter + 1 ];
                    if ( operand < MEMORY_SIZE && operand >= 0 ) {
                        int m_op = memory[ operand ];
                        if ( m_op < MEMORY_SIZE && m_op >= 0 ){

                            GtkWidget *container_window =
                            GTK_WIDGET(gtk_builder_get_object(builder,
                                        "CONTAINER_WINDOW"));

                            word_t number = show_number_input_dialog(NULL,
                                    container_window);   

                            memory[ m_op ] = number;
                            program_counter += 2;

                        } else {
                            snprintf(buffer,
                            sizeof(buffer),
                            "Invalid memory address %d",
                            m_op);
                            append_text_to_text_view( textview, buffer );
                            append_text_to_text_view( textview2, buffer );
                        }
                    }else {
                            snprintf(buffer,
                            sizeof(buffer),
                            "Invalid memory address %d",
                            operand);
                            append_text_to_text_view( textview, buffer );
                            append_text_to_text_view( textview2, buffer );
                    }
                }else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "Invalid memory address %d",
                    program_counter + 1);
                    append_text_to_text_view( textview, buffer );
                    append_text_to_text_view( textview2, buffer );
                }
            break;
            // STOP
            case STOP:
                append_text_to_text_view( textview,
                        "\nProgram terminated with exit status success.\n");
                append_text_to_text_view( textview2,
                        "\nProgram terminated with exit status success.\n");
            break;
            // ADD
            case ADD_DIRECT:
               if ( program_counter < MEMORY_SIZE - 1 ){
                    int operand = memory [ program_counter + 1 ];
                    if ( operand < MEMORY_SIZE && operand >= 0 ) {
                        if (abs( accumulator + memory[ operand ] ) > 
                            WORD_MAX ) {
                            snprintf(buffer,
                            sizeof(buffer),
                            "\nInteger overflow occurred when performing "
                            "instruction ADD with memory location %d, " 
                            "PC : %d, SP : %d \n",
                            memory[ operand ], program_counter, stack_pointer);
                            append_text_to_text_view(textview, buffer);
                            append_text_to_text_view(textview2, buffer);
                        }
                        accumulator += memory [ operand ];
                        program_counter += 2;
                    }else {
                        snprintf(buffer,
                        sizeof(buffer),
                        "Invalid memory address %d",
                        operand);
                        append_text_to_text_view( textview, buffer );
                        append_text_to_text_view( textview2, buffer );
                    }
               }else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "Invalid memory address %d",
                    program_counter + 1);
                    append_text_to_text_view( textview, buffer );
                    append_text_to_text_view( textview2, buffer );
               }
            break;
            case ADD_IMMEDIATE:
                if ( program_counter < MEMORY_SIZE - 1 ) {
                    int operand = memory [ program_counter + 1 ];
                    if (abs( accumulator + operand ) > 
                        WORD_MAX ) {
                        snprintf(buffer,
                        sizeof(buffer),
                        "\nInteger overflow occurred when performing "
                        "instruction ADD with memory location %d, " 
                        "PC : %d, SP : %d \n",
                        operand, program_counter, stack_pointer);
                        append_text_to_text_view(textview, buffer);
                        append_text_to_text_view(textview2, buffer);
                    }
                    accumulator += operand;
                    program_counter += 2;
                }else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "Invalid memory address %d",
                    program_counter + 1);
                    append_text_to_text_view( textview, buffer );
                    append_text_to_text_view( textview2, buffer );
                }
            break;
            case ADD_INDIRECT:
                if ( program_counter < MEMORY_SIZE - 1 ) {
                    int operand = memory[ program_counter + 1 ];
                    if ( operand < MEMORY_SIZE && operand >= 0 ) {
                        int m_op = memory[ operand ];
                        if ( m_op < MEMORY_SIZE && m_op >= 0 ){
                            if (abs( accumulator + memory[ m_op ] ) > 
                                WORD_MAX ) {
                                snprintf(buffer,
                                sizeof(buffer),
                                "\nInteger overflow occurred when performing "
                                "instruction ADD with memory location %d, " 
                                "PC : %d, SP : %d \n",
                                memory[ m_op ], program_counter, stack_pointer);
                                append_text_to_text_view(textview, buffer);
                                append_text_to_text_view(textview2, buffer);
                            }
                            accumulator += memory[ m_op ];
                            program_counter += 2;
                        } else if ( inst == ADD_INDIRECT ){
                            snprintf(buffer,
                            sizeof(buffer),
                            "Invalid memory address %d",
                            m_op);
                            append_text_to_text_view( textview, buffer );
                            append_text_to_text_view( textview2, buffer );
                        }
                    } else if ( inst == ADD_DIRECT ) {
                        snprintf(buffer,
                        sizeof(buffer),
                        "\nInvalid memory address for first "
                        "operand of instruction ADD at : %d\n",
                         operand );
                        append_text_to_text_view( textview, buffer );
                        append_text_to_text_view( textview2, buffer );
                    }
                } else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "\nNo operand found for instruction add at : memory[ %d ]\n",
                    program_counter + 1);
                    append_text_to_text_view( textview, buffer );
                    append_text_to_text_view( textview2, buffer );
                }
            break;
            
            // BRPOS
            case BRPOS_DIRECT:
            if ( accumulator > 0 ) {
                if ( program_counter < MEMORY_SIZE - 1 ) {
                    int operand = memory[ program_counter + 1 ];
                    if ( operand < MEMORY_SIZE && operand >= 0 ) {
                        program_counter = memory [ operand ];  
                    }else {
                        snprintf(buffer,
                        sizeof(buffer),
                        "Invalid memory address %d",
                        operand );
                        append_text_to_text_view( textview, buffer );
                        append_text_to_text_view( textview2, buffer );
                    }
                }else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "Invalid memory address %d",
                    program_counter + 1);
                    append_text_to_text_view( textview, buffer );
                    append_text_to_text_view( textview2, buffer );
                } 
            } else {
                program_counter += 2;
            }
            break;
            case BRPOS_IMMEDIATE:
            if ( accumulator > 0 ) {
               if ( program_counter < MEMORY_SIZE - 1 ){
                    int operand = memory [ program_counter + 1 ];
                    program_counter = operand;
               } else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "Invalid memory address %d",
                    program_counter + 1);
                    append_text_to_text_view( textview, buffer );
                    append_text_to_text_view( textview2, buffer );
               }
            } else {
                program_counter += 2;
            }
            break;
            case BRPOS_INDIRECT:
                if (accumulator > 0) {
                    if (program_counter < MEMORY_SIZE - 1) {
                        int operand = memory[program_counter + 1];
                        if (operand < MEMORY_SIZE && operand >= 0) {
                            int m_op = memory[operand];
                            if (m_op < MEMORY_SIZE && m_op >= 0) {
                                program_counter = memory[m_op];
                            } else {
                                snprintf(buffer,
                                sizeof(buffer),
                                "Invalid memory address %d",
                                m_op);
                                append_text_to_text_view(textview, buffer);
                                append_text_to_text_view(textview2, buffer);
                            }
                        } else {
                            snprintf(buffer,
                            sizeof(buffer),
                            "Invalid memory address %d",
                            operand);
                            append_text_to_text_view(textview, buffer);
                            append_text_to_text_view(textview2, buffer);
                        }
                    } else {
                        snprintf(buffer,
                        sizeof(buffer),
                        "No operand found for instruction BRPOS at : memory[%d]\n",
                        program_counter + 1);
                        append_text_to_text_view(textview, buffer);
                        append_text_to_text_view(textview2, buffer);
                    }
                } else {
                    program_counter += 2;
                }
            break;

            // BRZERO
            case BRZERO_DIRECT:
            if (accumulator == 0) {
                if (program_counter < MEMORY_SIZE - 1) {
                    int operand = memory[program_counter + 1];
                    if (operand < MEMORY_SIZE && operand >= 0) {
                        program_counter = memory[operand];
                    } else {
                        snprintf(buffer, sizeof(buffer),
                        "Invalid memory address %d", operand);
                        append_text_to_text_view(textview, buffer);
                        append_text_to_text_view(textview2, buffer);
                    }
                } else {
                    snprintf(buffer, sizeof(buffer),
                    "Invalid memory address %d", program_counter + 1);
                    append_text_to_text_view(textview, buffer);
                    append_text_to_text_view(textview2, buffer);
                }
            } else {
                program_counter += 2;
            }
            break;
            case BRZERO_IMMEDIATE:
            if (accumulator == 0) {
                if (program_counter < MEMORY_SIZE - 1) {
                    int operand = memory[program_counter + 1];
                    program_counter = operand;
                } else {
                    snprintf(buffer, sizeof(buffer),
                    "Invalid memory address %d", program_counter + 1);
                    append_text_to_text_view(textview, buffer);
                    append_text_to_text_view(textview2, buffer);
                }
            } else {
                program_counter += 2;
            }
            break;
            case BRZERO_INDIRECT:
            if (accumulator == 0) {
                if (program_counter < MEMORY_SIZE - 1) {
                    int operand = memory[program_counter + 1];
                    if (operand < MEMORY_SIZE && operand >= 0) {
                        int m_op = memory[operand];
                        if (m_op < MEMORY_SIZE && m_op >= 0) {
                            program_counter = memory[m_op];
                        } else {
                            snprintf(buffer, sizeof(buffer),
                            "Invalid memory address %d", m_op);
                            append_text_to_text_view(textview, buffer);
                            append_text_to_text_view(textview2, buffer);
                        }
                    } else {
                        snprintf(buffer, sizeof(buffer),
                        "Invalid memory address %d", operand);
                        append_text_to_text_view(textview, buffer);
                        append_text_to_text_view(textview2, buffer);
                    }
                } else {
                    snprintf(buffer, sizeof(buffer),
                    "Invalid memory address %d", program_counter + 1);
                    append_text_to_text_view(textview, buffer);
                    append_text_to_text_view(textview2, buffer);
                }
            } else {
                program_counter += 2;
            }
            break;

            // BRNEG
            case BRNEG_DIRECT:
            if (accumulator < 0) {
                if (program_counter < MEMORY_SIZE - 1) {
                    int operand = memory[program_counter + 1];
                    if (operand < MEMORY_SIZE && operand >= 0) {
                        program_counter = memory[operand];
                    } else {
                        snprintf(buffer, sizeof(buffer),
                        "Invalid memory address %d", operand);
                        append_text_to_text_view(textview, buffer);
                        append_text_to_text_view(textview2, buffer);
                    }
                } else {
                    snprintf(buffer, sizeof(buffer),
                    "Invalid memory address %d", program_counter + 1);
                    append_text_to_text_view(textview, buffer);
                    append_text_to_text_view(textview2, buffer);
                }
            } else {
                program_counter += 2;
            }
            break;
            case BRNEG_IMMEDIATE:
            if (accumulator < 0) {
                if (program_counter < MEMORY_SIZE - 1) {
                    int operand = memory[program_counter + 1];
                    program_counter = operand;
                } else {
                    snprintf(buffer, sizeof(buffer),
                    "Invalid memory address %d", program_counter + 1);
                    append_text_to_text_view(textview, buffer);
                    append_text_to_text_view(textview2, buffer);
                }
            } else {
                program_counter += 2;
            }
            break;
            case BRNEG_INDIRECT:
            if (accumulator < 0) {
                if (program_counter < MEMORY_SIZE - 1) {
                    int operand = memory[program_counter + 1];
                    if (operand < MEMORY_SIZE && operand >= 0) {
                        int m_op = memory[operand];
                        if (m_op < MEMORY_SIZE && m_op >= 0) {
                            program_counter = memory[m_op];
                        } else {
                            snprintf(buffer, sizeof(buffer),
                            "Invalid memory address %d", m_op);
                            append_text_to_text_view(textview, buffer);
                            append_text_to_text_view(textview2, buffer);
                        }
                    } else {
                        snprintf(buffer, sizeof(buffer),
                        "Invalid memory address %d", operand);
                        append_text_to_text_view(textview, buffer);
                        append_text_to_text_view(textview2, buffer);
                    }
                } else {
                    snprintf(buffer, sizeof(buffer),
                    "Invalid memory address %d", program_counter + 1);
                    append_text_to_text_view(textview, buffer);
                    append_text_to_text_view(textview2, buffer);
                }
            } else {
                program_counter += 2;
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
                            append_text_to_text_view( textview2, lu);
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
                        append_text_to_text_view( textview2, buffer );
                    }
                } else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "\nNo operand found for instruction WRITE at : memory[ %d ]\n",
                    program_counter + 1);
                    append_text_to_text_view( textview, buffer );
                    append_text_to_text_view( textview2, buffer );
                }
            break;
            // MULT
            case MULT_DIRECT:
            if (program_counter < MEMORY_SIZE - 1) {
                int operand = memory[program_counter + 1];
                if (operand < MEMORY_SIZE && operand >= 0) {
                    if (abs(accumulator * memory[operand]) > WORD_MAX) {
                        snprintf(buffer, sizeof(buffer),
                        "Integer overflow occurred when multiplying"
                        "accumulator with memory[%d]", operand);
                        append_text_to_text_view(textview, buffer);
                        append_text_to_text_view(textview2, buffer);
                    } 
                    accumulator *= memory[operand];
                    program_counter += 2;
                } else {
                    snprintf(buffer, sizeof(buffer),
                    "Invalid memory address %d", operand);
                    append_text_to_text_view(textview, buffer);
                    append_text_to_text_view(textview2, buffer);
                }
            } else {
                snprintf(buffer, sizeof(buffer),
                "Invalid memory address %d", program_counter + 1);
                append_text_to_text_view(textview, buffer);
                append_text_to_text_view(textview2, buffer);
            }
            break;
            case MULT_IMMEDIATE:
            if (program_counter < MEMORY_SIZE - 1) {
                int operand = memory[program_counter + 1];
                if (abs(accumulator * operand) > WORD_MAX) {
                    snprintf(buffer, sizeof(buffer),
                    "Integer overflow occurred when multiplying"
                    "accumulator with %d", operand);
                    append_text_to_text_view(textview, buffer);
                    append_text_to_text_view(textview2, buffer);
                }
                accumulator *= operand;
                program_counter += 2;
            } else {
                snprintf(buffer, sizeof(buffer),
                "Invalid memory address %d", program_counter + 1);
                append_text_to_text_view(textview, buffer);
                append_text_to_text_view(textview2, buffer);
            }
            break;
            case MULT_INDIRECT:
            if (program_counter < MEMORY_SIZE - 1) {
                int operand = memory[program_counter + 1];
                if (operand < MEMORY_SIZE && operand >= 0) {
                    int m_op = memory[operand];
                    if (m_op < MEMORY_SIZE && m_op >= 0) {
                        if (abs(accumulator * memory[m_op]) > WORD_MAX) {
                            snprintf(buffer, sizeof(buffer), "Integer overflow"
                            "occurred when multiplying accumulator"
                            "with memory[%d]", m_op);
                            append_text_to_text_view(textview, buffer);
                            append_text_to_text_view(textview2, buffer);
                        } else {
                            accumulator *= memory[m_op];
                            program_counter += 2;
                        }
                    } else {
                        snprintf(buffer, sizeof(buffer),
                        "Invalid memory address %d", m_op);
                        append_text_to_text_view(textview, buffer);
                        append_text_to_text_view(textview2, buffer);
                    }
                } else {
                    snprintf(buffer, sizeof(buffer),
                    "Invalid memory address %d", operand);
                    append_text_to_text_view(textview, buffer);
                    append_text_to_text_view(textview2, buffer);
                }
            } else {
                snprintf(buffer, sizeof(buffer),
                "Invalid memory address %d", program_counter + 1);
                append_text_to_text_view(textview, buffer);
                append_text_to_text_view(textview2, buffer);
            }
            break;           
            // DIV
            case DIV_DIRECT:
            if (program_counter < MEMORY_SIZE - 1) {
                int operand = memory[program_counter + 1];
                if (operand < MEMORY_SIZE && operand >= 0) {
                    if (memory[operand] == 0) {
                        snprintf(buffer, sizeof(buffer),
                        "Division by zero error at memory[%d]", operand);
                        append_text_to_text_view(textview, buffer);
                        append_text_to_text_view(textview2, buffer);
                        reset( (void*) 0, builder );
                        running = false;
                    } 
                    if (abs(accumulator / memory[operand]) > WORD_MAX) {
                        snprintf(buffer, sizeof(buffer),
                        "Integer overflow occurred when dividing"
                        "accumulator by memory[%d]", operand);
                        append_text_to_text_view(textview, buffer);
                        append_text_to_text_view(textview2, buffer);
                    }
                    accumulator /= memory[operand];
                    program_counter += 2;
                } else {
                    snprintf(buffer, sizeof(buffer),
                    "Invalid memory address %d", operand);
                    append_text_to_text_view(textview, buffer);
                    append_text_to_text_view(textview2, buffer);
                }
            } else {
                snprintf(buffer, sizeof(buffer),
                "Invalid memory address %d", program_counter + 1);
                append_text_to_text_view(textview, buffer);
                append_text_to_text_view(textview2, buffer);
            }
            break;
            case DIV_INDIRECT:
            if (program_counter < MEMORY_SIZE - 1) {
                int operand = memory[program_counter + 1];
                if (operand < MEMORY_SIZE && operand >= 0) {
                    int m_op = memory[operand];
                    if (m_op < MEMORY_SIZE && m_op >= 0) {
                        if (memory[m_op] == 0) {
                            snprintf(buffer, sizeof(buffer),
                            "Division by zero error at memory[%d]", m_op);
                            append_text_to_text_view(textview, buffer);
                            append_text_to_text_view(textview2, buffer);
                            reset( (void *)0, builder);
                            running = false;
                        } 
                        if (abs(accumulator / memory[m_op]) > WORD_MAX) {
                            snprintf(buffer, sizeof(buffer),
                            "Integer overflow occurred when dividing"
                            "accumulator by memory[%d]", m_op);
                            append_text_to_text_view(textview, buffer);
                            append_text_to_text_view(textview2, buffer);
                        }
                        accumulator /= memory[m_op];
                        program_counter += 2;
                    } else {
                        snprintf(buffer, sizeof(buffer),
                        "Invalid memory address %d", m_op);
                        append_text_to_text_view(textview, buffer);
                        append_text_to_text_view(textview2, buffer);
                    }
                } else {
                    snprintf(buffer, sizeof(buffer),
                    "Invalid memory address %d", operand);
                    append_text_to_text_view(textview, buffer);
                    append_text_to_text_view(textview2, buffer);
                }
            } else {
                snprintf(buffer, sizeof(buffer),
                "Invalid memory address %d", program_counter + 1);
                append_text_to_text_view(textview, buffer);
                append_text_to_text_view(textview2, buffer);
            }
            break;
            case DIV_IMMEDIATE:
            if (program_counter < MEMORY_SIZE - 1) {
                int operand = memory[program_counter + 1];
                if (operand == 0) {
                    snprintf(buffer, sizeof(buffer),
                    "Division by zero error at memory[%d]", operand);
                    append_text_to_text_view(textview, buffer);
                    append_text_to_text_view(textview2, buffer);
                    reset( (void*)0, builder );
                    running = false;
                } 
                if (abs(accumulator / operand) > WORD_MAX) {
                    snprintf(buffer, sizeof(buffer), 
                    "Integer overflow occurred when dividing"
                    "accumulator by %d", operand);
                    append_text_to_text_view(textview, buffer);
                    append_text_to_text_view(textview2, buffer);
                } 
                accumulator /= operand;
                program_counter += 2;
            } else {
                snprintf(buffer, sizeof(buffer),
                "Invalid memory address %d", program_counter + 1);
                append_text_to_text_view(textview, buffer);
                append_text_to_text_view(textview2, buffer);
            }
            break;
            // BR
           case BR_DIRECT:
            if (program_counter < MEMORY_SIZE - 1) {
                int operand = memory[program_counter + 1];
                if (operand < MEMORY_SIZE && operand >= 0) {
                    program_counter = memory[operand];
                } else {
                    snprintf(buffer, sizeof(buffer),
                    "Invalid memory address %d", operand);
                    append_text_to_text_view(textview, buffer);
                    append_text_to_text_view(textview2, buffer);
                }
            } else {
                snprintf(buffer, sizeof(buffer),
                "Invalid memory address %d", program_counter + 1);
                append_text_to_text_view(textview, buffer);
                append_text_to_text_view(textview2, buffer);
            }
            break;
            case BR_INDIRECT:
            if (program_counter < MEMORY_SIZE - 1) {
                int operand = memory[program_counter + 1];
                if (operand < MEMORY_SIZE && operand >= 0) {
                    int m_op = memory[operand];
                    if (m_op < MEMORY_SIZE && m_op >= 0) {
                        program_counter = memory[m_op];
                    } else {
                        snprintf(buffer, sizeof(buffer),
                        "Invalid memory address %d", m_op);
                        append_text_to_text_view(textview, buffer);
                        append_text_to_text_view(textview2, buffer);
                    }
                } else {
                    snprintf(buffer, sizeof(buffer),
                    "Invalid memory address %d", operand);
                    append_text_to_text_view(textview, buffer);
                    append_text_to_text_view(textview2, buffer);
                }
            } else {
                snprintf(buffer, sizeof(buffer),
                "Invalid memory address %d", program_counter + 1);
                append_text_to_text_view(textview, buffer);
                append_text_to_text_view(textview2, buffer);
            }
            break;
            case BR_IMMEDIATE:
            if (program_counter < MEMORY_SIZE - 1) {
                int operand = memory[program_counter + 1];
                program_counter = operand;
            } else {
                snprintf(buffer, sizeof(buffer),
                "Invalid memory address %d", program_counter + 1);
                append_text_to_text_view(textview, buffer);
                append_text_to_text_view(textview2, buffer);
            }
            break;

            // PUT
            case PUT_DIRECT:
            if ( program_counter < MEMORY_SIZE - 1 ) {
                int operand = memory [ program_counter + 1 ];
                if ( operand < MEMORY_SIZE && operand >= 0 ){
                    char number[256];
                    snprintf(number, 256, "%d", memory[ operand ]);
                    append_text_to_text_view(textview, number); 
                    append_text_to_text_view(textview2, number); 
                    program_counter+=2;
                }else {
                    snprintf(buffer, sizeof(buffer),
                    "Invalid memory address %d", operand);
                    append_text_to_text_view(textview, buffer);
                    append_text_to_text_view(textview2, buffer);
                }
            }else { 
                snprintf(buffer, sizeof(buffer),
                "Invalid memory address %d", program_counter + 1);
                append_text_to_text_view(textview, buffer);
                append_text_to_text_view(textview2, buffer);
            }
            break;
            case PUT_IMMEDIATE:
                if ( program_counter < MEMORY_SIZE - 1 ) {
                    int operand = memory[ program_counter + 1];
                    char number[256];
                    snprintf(number, 256, "%d", operand );
                    append_text_to_text_view(textview, number); 
                    append_text_to_text_view(textview2, number); 
                    program_counter+=2;
                }else {
                    snprintf(buffer, sizeof(buffer),
                    "Invalid memory address %d", program_counter + 1);
                    append_text_to_text_view(textview, buffer);
                    append_text_to_text_view(textview2, buffer);
                }
            break;
            case PUT_INDIRECT:
                if ( program_counter < MEMORY_SIZE - 1 ) {
                    int operand = memory[program_counter + 1];
                    if ( operand < MEMORY_SIZE && operand >= 0) {
                        int m_op = memory[ operand ];
                        if ( m_op < MEMORY_SIZE && m_op >= 0 ) {
                            char number[256];
                            snprintf(number, 256, "%d", memory[ m_op ]);
                            append_text_to_text_view(textview, number); 
                            append_text_to_text_view(textview2, number); 
                            program_counter+=2;
                        }else {
                            snprintf(buffer,
                            sizeof(buffer),
                            "Invalid memory address %d",
                            m_op);
                            append_text_to_text_view(textview, buffer);
                            append_text_to_text_view(textview2, buffer);
                        }
                    }else {
                        snprintf(buffer,
                        sizeof(buffer),
                        "Invalid memory address %d",
                        operand);
                        append_text_to_text_view(textview, buffer);
                        append_text_to_text_view(textview2, buffer);
                    }
                }else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "Invalid memory address %d",
                    program_counter + 1);
                    append_text_to_text_view(textview, buffer);
                    append_text_to_text_view(textview2, buffer);
                }
            break;

            // CALL
            case CALL_DIRECT:
            if ( program_counter < MEMORY_SIZE - 1 ){
                int operand = memory [ program_counter + 1 ];
                if ( operand < MEMORY_SIZE && operand >= 0 ){
                    bool status = push(program_counter + 2);
                    if (!status) {
                        snprintf(buffer,
                        sizeof(buffer),
                        "\nStack overflow occurred  PC : %d "
                        "SP : %d \n",
                        program_counter, stack_pointer );
                        append_text_to_text_view( textview, buffer );
                        append_text_to_text_view( textview2, buffer );
                    }
                    program_counter = memory[ operand ]; 
                } else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "Invalid memory address %d",
                    operand);
                    append_text_to_text_view(textview, buffer);
                    append_text_to_text_view(textview2, buffer);
                }
            }else {
                snprintf(buffer,
                sizeof(buffer),
                "Invalid memory address %d",
                program_counter + 1);
                append_text_to_text_view(textview, buffer);
                append_text_to_text_view(textview2, buffer);
            }
            break;
            case CALL_IMMEDIATE:
            if ( program_counter < MEMORY_SIZE - 1 ){
                int operand = memory [ program_counter + 1 ];
                bool status = push(program_counter + 2);
                if (!status) {
                    snprintf(buffer,
                    sizeof(buffer),
                    "\nStack overflow occurred  PC : %d "
                    "SP : %d \n",
                    program_counter, stack_pointer );
                    append_text_to_text_view( textview, buffer );
                    append_text_to_text_view( textview2, buffer );
                }
                program_counter = operand; 
            }else {
                snprintf(buffer,
                sizeof(buffer),
                "Invalid memory address %d",
                program_counter + 1);
                append_text_to_text_view(textview, buffer);
                append_text_to_text_view(textview2, buffer);
            }
            break;
            case CALL_INDIRECT:
                if ( program_counter < MEMORY_SIZE - 1 ) {
                    int operand = memory[program_counter + 1];
                    if ( operand < MEMORY_SIZE && operand >= 0) {
                        int m_op = memory[operand];
                        if (m_op < MEMORY_SIZE && m_op >= 0) {
                            bool status = push(program_counter + 2);
                            if (!status) {
                                snprintf(buffer,
                                sizeof(buffer),
                                "\nStack overflow occurred  PC : %d "
                                "SP : %d \n",
                                program_counter, stack_pointer );
                                append_text_to_text_view( textview, buffer );
                                append_text_to_text_view( textview2, buffer );
                            }
                            program_counter = memory[ m_op ]; 
                        }else {
                            snprintf(buffer,
                            sizeof(buffer),
                            "Invalid memory address %d",
                            m_op);
                            append_text_to_text_view(textview, buffer);
                            append_text_to_text_view(textview2, buffer);
                        }
                    }else {
                        snprintf(buffer,
                        sizeof(buffer),
                        "Invalid memory address %d",
                        operand);
                        append_text_to_text_view(textview, buffer);
                        append_text_to_text_view(textview2, buffer);
                    }
                }else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "Invalid memory address %d",
                    program_counter + 1);
                    append_text_to_text_view(textview, buffer);
                    append_text_to_text_view(textview2, buffer);
                }
            break;
            // COPY
            case COPY_DD:
            if (program_counter < MEMORY_SIZE - 2) {
                    int op1 = memory[program_counter + 1];
                    int op2 = memory[program_counter + 2];
                    if (op1 < MEMORY_SIZE && op1 >= 0 &&
                        op2 < MEMORY_SIZE && op2 >= 0) {
                        memory[op1] = memory[op2];
                        program_counter += 3;
                    } else {
                        snprintf(buffer, sizeof(buffer),
                        "Invalid memory address for COPY_DD at: %d or %d", op1, op2);
                        append_text_to_text_view(textview, buffer);
                        append_text_to_text_view(textview2, buffer);
                    }
                } else {
                    snprintf(buffer, sizeof(buffer),
                    "No operand found for instruction COPY_DD"
                    "at: memory[%d] or memory[%d]",
                        program_counter + 1, program_counter + 2);
                    append_text_to_text_view(textview, buffer);
                    append_text_to_text_view(textview2, buffer);
                }
            break;
            case COPY_DI:
            if (program_counter < MEMORY_SIZE - 2) {
                    int op1 = memory[program_counter + 1];
                    int op2 = memory[program_counter + 2];
                    if (op1 < MEMORY_SIZE && op1 >= 0 &&
                        op2 < MEMORY_SIZE && op2 >= 0) {
                        int m_op2 = memory[ op2 ];
                        if ( m_op2 < MEMORY_SIZE && m_op2 >= 0 ){
                            memory[op1] = memory[ m_op2 ];
                            program_counter += 3;
                        }else {
                            snprintf(buffer,
                            sizeof(buffer),
                            "Invalid memory address %d",
                            m_op2);
                            append_text_to_text_view(textview, buffer);
                            append_text_to_text_view(textview2, buffer);
                        }
                    } else {
                        snprintf(buffer, sizeof(buffer),
                        "Invalid memory address for COPY_DI at: %d or %d", op1, op2);
                        append_text_to_text_view(textview, buffer);
                        append_text_to_text_view(textview2, buffer);
                    }
                } else {
                    snprintf(buffer, sizeof(buffer),
                    "No operand found for instruction"
                    "COPY_DI at: memory[%d] or memory[%d]",
                    program_counter + 1, program_counter + 2);
                    append_text_to_text_view(textview, buffer);
                    append_text_to_text_view(textview2, buffer);
                }
            break;
            case COPY_DIm:
            if (program_counter < MEMORY_SIZE - 2) {
                    int op1 = memory[program_counter + 1];
                    int op2 = memory[program_counter + 2];
                    if (op1 < MEMORY_SIZE && op1 >= 0 ) {
                        memory[op1] = op2;
                        program_counter += 3;
                    } else {
                        snprintf(buffer, sizeof(buffer),
                        "Invalid memory address for COPY_DIm at: %d or %d", op1, op2);
                        append_text_to_text_view(textview, buffer);
                        append_text_to_text_view(textview2, buffer);
                    }
                } else {
                    snprintf(buffer, sizeof(buffer),
                    "No operand found for instruction"
                    "COPY_DIm at: memory[%d] or memory[%d]",
                    program_counter + 1, program_counter + 2);
                    append_text_to_text_view(textview, buffer);
                    append_text_to_text_view(textview2, buffer);
                }
            break;
            case COPY_ID:
            if (program_counter < MEMORY_SIZE - 2) {
                    int op1 = memory[program_counter + 1];
                    int op2 = memory[program_counter + 2];
                    if (op1 < MEMORY_SIZE && op1 >= 0 &&
                        op2 < MEMORY_SIZE && op2 >= 0) {
                        int m_op1 = memory[ op1 ];
                        if ( m_op1 < MEMORY_SIZE && m_op1 >= 0 ){
                            memory[m_op1] = memory[op2];
                            program_counter += 3;
                        }else {
                            snprintf(buffer,
                            sizeof(buffer),
                            "Invalid memory address %d",
                            m_op1);
                            append_text_to_text_view(textview, buffer);
                            append_text_to_text_view(textview2, buffer);
                        }
                    } else {
                        snprintf(buffer, sizeof(buffer),
                        "Invalid memory address for COPY_ID at: %d or %d", op1, op2);
                        append_text_to_text_view(textview, buffer);
                        append_text_to_text_view(textview2, buffer);
                    }
                } else {
                    snprintf(buffer, sizeof(buffer),
                    "No operand found for instruction"
                    "COPY_ID at: memory[%d] or memory[%d]",
                    program_counter + 1, program_counter + 2);
                    append_text_to_text_view(textview, buffer);
                    append_text_to_text_view(textview2, buffer);
                }
            break;
            case COPY_II:
            if (program_counter < MEMORY_SIZE - 2) {
                    int op1 = memory[program_counter + 1];
                    int op2 = memory[program_counter + 2];
                    if (op1 < MEMORY_SIZE && op1 >= 0 &&
                        op2 < MEMORY_SIZE && op2 >= 0) {
                        int m_op1 = memory [ op1 ];
                        int m_op2 = memory [ op2 ];
                        if ( m_op1 < MEMORY_SIZE && m_op1 >= 0 &&
                             m_op2 < MEMORY_SIZE && m_op2 >= 0 ) {
                            memory[m_op1] = memory[m_op2];
                            program_counter += 3;
                        } else {
                            snprintf(buffer,
                            sizeof(buffer),
                            "Invalid memory address %d -- %d",
                            m_op1, m_op2);
                            append_text_to_text_view(textview, buffer);
                            append_text_to_text_view(textview2, buffer);
                        }
                    } else {
                        snprintf(buffer, sizeof(buffer),
                        "Invalid memory address for COPY_II at: %d or %d", op1, op2);
                        append_text_to_text_view(textview, buffer);
                        append_text_to_text_view(textview2, buffer);
                    }
                } else {
                    snprintf(buffer, sizeof(buffer),
                    "No operand found for instruction"
                    "COPY_II at: memory[%d] or memory[%d]",
                    program_counter + 1, program_counter + 2);
                    append_text_to_text_view(textview, buffer);
                    append_text_to_text_view(textview2, buffer);
                }
            break;
            case COPY_IIm:
            if (program_counter < MEMORY_SIZE - 2) {
                    int op1 = memory[program_counter + 1];
                    int op2 = memory[program_counter + 2];
                    if (op1 < MEMORY_SIZE && op1 >= 0 ) {
                        int m_op1 = memory [ op1 ];
                        if ( m_op1 < MEMORY_SIZE && m_op1 >= 0 ) {
                            memory[m_op1] = op2;
                            program_counter += 3;
                        }else {
                            snprintf(buffer,
                            sizeof(buffer),
                            "Invalid memory address %d",
                            m_op1 );
                            append_text_to_text_view(textview, buffer);
                            append_text_to_text_view(textview2, buffer);
                        }
                    } else {
                        snprintf(buffer, sizeof(buffer),
                        "Invalid memory address for COPY_IIm at: %d or %d", op1, op2);
                        append_text_to_text_view(textview, buffer);
                        append_text_to_text_view(textview2, buffer);
                    }
                } else {
                    snprintf(buffer, sizeof(buffer),
                    "No operand found for instruction"
                    "COPY_IIm at: memory[%d] or memory[%d]",
                    program_counter + 1, program_counter + 2);
                    append_text_to_text_view(textview, buffer);
                    append_text_to_text_view(textview2, buffer);
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
            // SUB
            case SUB_DIRECT:
            if (program_counter < MEMORY_SIZE - 1) {
                int operand = memory[program_counter + 1];
                if (operand < MEMORY_SIZE && operand >= 0) {
                    if (abs(accumulator - memory[operand]) > WORD_MAX) {
                        snprintf(buffer, sizeof(buffer),
                        "Integer overflow occurred when subtracting"
                        "memory[%d] from accumulator", operand);
                        append_text_to_text_view(textview, buffer);
                        append_text_to_text_view(textview2, buffer);
                    }
                    accumulator -= memory[operand];
                    program_counter += 2;
                } else {
                    snprintf(buffer, sizeof(buffer),
                    "Invalid memory address %d", operand);
                    append_text_to_text_view(textview, buffer);
                    append_text_to_text_view(textview2, buffer);
                }
            } else {
                snprintf(buffer, sizeof(buffer),
                "Invalid memory address %d", program_counter + 1);
                append_text_to_text_view(textview, buffer);
                append_text_to_text_view(textview2, buffer);
            }
            break;
            case SUB_INDIRECT:
            if (program_counter < MEMORY_SIZE - 1) {
                int operand = memory[program_counter + 1];
                if (operand < MEMORY_SIZE && operand >= 0) {
                    int m_op = memory[operand];
                    if (m_op < MEMORY_SIZE && m_op >= 0) {
                        if (abs(accumulator - memory[m_op]) > WORD_MAX) {
                            snprintf(buffer, sizeof(buffer),
                            "Integer overflow occurred when subtracting"
                            "memory[%d] from accumulator", m_op);
                            append_text_to_text_view(textview, buffer);
                            append_text_to_text_view(textview2, buffer);
                        }
                        accumulator -= memory[m_op];
                        program_counter += 2;
                    } else {
                        snprintf(buffer, sizeof(buffer),
                        "Invalid memory address %d", m_op);
                        append_text_to_text_view(textview, buffer);
                        append_text_to_text_view(textview2, buffer);
                    }
                } else {
                    snprintf(buffer, sizeof(buffer),
                    "Invalid memory address %d", operand);
                    append_text_to_text_view(textview, buffer);
                    append_text_to_text_view(textview2, buffer);
                }
            } else {
                snprintf(buffer, sizeof(buffer),
                "Invalid memory address %d", program_counter + 1);
                append_text_to_text_view(textview, buffer);
                append_text_to_text_view(textview2, buffer);
            }
            break;
            case SUB_IMMEDIATE:
            if (program_counter < MEMORY_SIZE - 1) {
                int operand = memory[program_counter + 1];
                if (abs(accumulator - operand) > WORD_MAX) {
                    snprintf(buffer, sizeof(buffer),
                    "Integer overflow occurred when subtracting"
                    "%d from accumulator", operand);
                    append_text_to_text_view(textview, buffer);
                    append_text_to_text_view(textview2, buffer);
                } 
                accumulator -= operand;
                program_counter += 2;
            } else {
                snprintf(buffer, sizeof(buffer),
                "Invalid memory address %d", program_counter + 1);
                append_text_to_text_view(textview, buffer);
                append_text_to_text_view(textview2, buffer);
            }
            break;           
            // UNKNOWN 
            default:
                snprintf(buffer,
                sizeof(buffer),
                "\nNot an instruction at memory address : %d \n",
                program_counter);
                append_text_to_text_view( textview, buffer );
                append_text_to_text_view( textview2, buffer );
            break;
        }
    } else  {
        textview =
        GTK_TEXT_VIEW(gtk_builder_get_object(builder, "console"));
        append_text_to_text_view( textview, "\nMemory limit reached. \n" );
        append_text_to_text_view( textview2, "\nMemory limit reached. \n" );
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

void colorize_token(GtkTextBuffer *buffer,
        int token_start, int token_end, const char *color) {
    GtkTextIter start_iter, end_iter;

    gtk_text_buffer_get_iter_at_offset(buffer, &start_iter, token_start);
    gtk_text_buffer_get_iter_at_offset(buffer, &end_iter, token_end);

    GtkTextTag *tag =
        gtk_text_buffer_create_tag(buffer, NULL, "foreground", color, NULL);

    gtk_text_buffer_apply_tag(buffer, tag, &start_iter, &end_iter);
}

void read_and_insert_file_content(GtkBuilder *builder, const char *filename) {
    GtkWidget *text_view = GTK_WIDGET(gtk_builder_get_object(builder, "console2"));

    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        g_print("Erro ao abrir o arquivo");
        return;
    }

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    program_t *program = createProgram(file); 
    fclose(file);
    tokenize(program);
    printTokens(program);

    gtk_text_buffer_set_text(buffer, program->source, -1);
    for( int i=0; i < program->n_tokens - 1; i++ )
    {
        int type = program->tokens[i].type;
        const char *color = tok_colors[type];

        if ( color != NULL && type != TOK_UNKNOWN ) 
        {
            // Offset coloring is broken currently
            // idk why, a fix is to subtract one from the first argument
            // program->tokens[i].offset - 1
            // but that is not wanted.
            if ( program->tokens[i].offset - 1 > 0 )
                colorize_token( buffer, program->tokens[i].offset - 1, 
                        program->tokens[i].offset + strlen(program->tokens[i].token),
                        color );
            else 
                colorize_token( buffer, program->tokens[i].offset, 
                        program->tokens[i].offset + strlen(program->tokens[i].token),
                        color );
        }
    }
}

void open_file(GtkButton *button, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkFileChooserButton *btn = GTK_FILE_CHOOSER_BUTTON(button);
    
    char *filename =
        gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(btn));

    current_program = 
        malloc( strlen(filename) + 1 );
    if ( current_program == NULL )
    {
    }

    strcpy( current_program, filename);

    g_print("Arquivo selecionado: %s\n", filename);

    read_and_insert_file_content(builder, filename);

    g_free(filename);
}

void run(GtkWidget *widget, gpointer data) {

    running = true;
    user_data_t *ud = data;
    GtkTextView *textview = GTK_TEXT_VIEW(gtk_builder_get_object(ud->builder,
                "console"));
    GtkTextView *textview2 = GTK_TEXT_VIEW(gtk_builder_get_object(ud->builder,
                "console1"));
    int inst;
    do {
        if ( program_counter < MEMORY_SIZE ) {
            inst = memory[ program_counter ];
            step( (void *)0, ud);
        } else { 
           append_text_to_text_view(textview,
                   "SEGFAULT: PC outside of memory range"); 
           append_text_to_text_view(textview2,
                   "SEGFAULT: PC outside of memory range"); 
           break;
        }
    }while ( inst != STOP && ( mop != MOP_NOT_DEFINED ) && running );

}

