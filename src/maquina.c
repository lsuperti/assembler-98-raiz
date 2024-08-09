
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
    if ( enum_str[inst] != NULL ){
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
        &&inst != COPY_IIm ) {

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

    if ( enum_str[inst] != NULL ) {
        free(write_buff);
    }

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
            case LOAD_INDIRECT:
            case LOAD_IMMEDIATE:
            case LOAD_DIRECT:            
                if ( program_counter < MEMORY_SIZE - 1 ) {
                    int operand = memory[ program_counter + 1 ];
                    if ( operand < MEMORY_SIZE && operand >= 0 ) {
                        int m_op = memory[ operand ];
                        if ( m_op < MEMORY_SIZE && m_op >= 0 ){
                            switch(inst) {
                                case LOAD_DIRECT:
                                    accumulator = memory[ operand ];
                                break;
                                case LOAD_INDIRECT:
                                    accumulator = memory[ m_op ];
                                break;
                                case LOAD_IMMEDIATE:
                                    accumulator = operand;
                                break;
                            }
                            program_counter += 2;
                        } else {
                            snprintf(buffer,
                            sizeof(buffer),
                            "Invalid memory address %d",
                            m_op);
                            append_text_to_text_view( textview, buffer );
                        }
                    } else {
                        snprintf(buffer,
                        sizeof(buffer),
                        "Invalid memory address %d",
                        operand );
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
            case STORE_INDIRECT:
                if ( program_counter < MEMORY_SIZE - 1 ) {
                    int operand = memory [ program_counter + 1 ];
                    if ( operand < MEMORY_SIZE && operand >= 0 ){
                        int m_op = memory[ operand ];
                        if ( m_op < MEMORY_SIZE && m_op >= 0 ){
                            switch(inst) {
                                case STORE_DIRECT:
                                    memory[ operand ] = accumulator;
                                break;
                                case STORE_INDIRECT:
                                    memory[ m_op ] = accumulator;
                                break;
                            }
                            program_counter += 2;
                        } else {
                            snprintf(buffer,
                            sizeof(buffer),
                            "Invalid memory address %d",
                            m_op);
                            append_text_to_text_view( textview, buffer );
                        }
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
            case READ_INDIRECT:
                if ( program_counter < MEMORY_SIZE - 1 ) {
                    int operand = memory [ program_counter + 1 ];
                    if ( operand < MEMORY_SIZE && operand >= 0 ) {
                        GtkWidget *container_window =
                        GTK_WIDGET(gtk_builder_get_object(builder, "CONTAINER_WINDOW"));
                        // This is blocking vvvv
                        word_t number = show_number_input_dialog(NULL,
                                container_window);   

                        int m_op = memory[ operand ];
                        if ( m_op < MEMORY_SIZE && m_op >= 0 ){
                            switch(inst) {
                                case READ_DIRECT:
                                    memory[ operand ] = number;
                                break;
                                case READ_INDIRECT:
                                    memory[ m_op ] = number;
                                break;
                            }
                            program_counter += 2;
                        } else {
                            snprintf(buffer,
                            sizeof(buffer),
                            "Invalid memory address %d",
                            m_op);
                            append_text_to_text_view( textview, buffer );
                        }
                    }else {
                            snprintf(buffer,
                            sizeof(buffer),
                            "Invalid memory address %d",
                            operand);
                            append_text_to_text_view( textview, buffer );
                    }
                }else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "Invalid memory address %d",
                    program_counter + 1);
                    append_text_to_text_view( textview, buffer );
                }
            break;
            // STOP
            case STOP:
                append_text_to_text_view( textview,
                        "\nProgram terminated with exit status success.\n");
            break;
            // ADD
            case ADD_DIRECT:
            case ADD_INDIRECT:
            case ADD_IMMEDIATE:
                if ( program_counter < MEMORY_SIZE - 1 ) {
                    int operand = memory[ program_counter + 1 ];
                    if ( operand < MEMORY_SIZE && operand >= 0 ) {
                        int m_op = memory[ operand ];
                        if ( m_op < MEMORY_SIZE && m_op >= 0 ){
                            switch(inst) {
                                case ADD_DIRECT:
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
                                break;
                                case ADD_INDIRECT:
                                    if (abs( accumulator + memory[ m_op ] ) > 
                                        WORD_MAX ) {
                                        snprintf(buffer,
                                        sizeof(buffer),
                                        "\nInteger overflow occurred when performing "
                                        "instruction ADD with memory location %d, " 
                                        "PC : %d, SP : %d \n",
                                        operand, program_counter, stack_pointer);
                                        append_text_to_text_view(textview, buffer);
                                    }
                                    accumulator += memory[ m_op ];
                                break;
                                case ADD_IMMEDIATE:
                                    if (abs( accumulator + operand ) > 
                                        WORD_MAX ) {
                                        snprintf(buffer,
                                        sizeof(buffer),
                                        "\nInteger overflow occurred when performing "
                                        "instruction ADD with memory location %d, " 
                                        "PC : %d, SP : %d \n",
                                        operand, program_counter, stack_pointer);
                                        append_text_to_text_view(textview, buffer);
                                    }
                                    accumulator += operand;
                                break;
                            }
                            program_counter += 2;
                        } else {
                            snprintf(buffer,
                            sizeof(buffer),
                            "Invalid memory address %d",
                            m_op);
                            append_text_to_text_view( textview, buffer );
                        }
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
            
            // BRPOS
            case BRPOS_DIRECT:
            case BRPOS_INDIRECT:
            case BRPOS_IMMEDIATE:
                if (accumulator > 0) {
                    if (program_counter < MEMORY_SIZE - 1) {
                        int operand = memory[program_counter + 1];
                        if (operand < MEMORY_SIZE && operand >= 0) {
                            int m_op = memory[operand];
                            if (m_op < MEMORY_SIZE && m_op >= 0) {
                                switch (inst) {
                                    case BRPOS_DIRECT:
                                        program_counter = memory[operand];
                                    break;
                                    case BRPOS_INDIRECT:
                                        program_counter = memory[m_op];
                                    break;
                                    case BRPOS_IMMEDIATE:
                                        program_counter = operand;
                                    break;
                                }
                            } else {
                                snprintf(buffer,
                                sizeof(buffer),
                                "Invalid memory address %d",
                                m_op);
                                append_text_to_text_view(textview, buffer);
                            }
                        } else {
                            snprintf(buffer,
                            sizeof(buffer),
                            "Invalid memory address %d",
                            operand);
                            append_text_to_text_view(textview, buffer);
                        }
                    } else {
                        snprintf(buffer,
                        sizeof(buffer),
                        "No operand found for instruction BRPOS at : memory[%d]\n",
                        program_counter + 1);
                        append_text_to_text_view(textview, buffer);
                    }
                } else {
                    program_counter += 2;
                }
            break;

            // BRZERO
            case BRZERO_DIRECT:
            case BRZERO_INDIRECT:
            case BRZERO_IMMEDIATE:
                if (accumulator == 0) {
                    if (program_counter < MEMORY_SIZE - 1) {
                        int operand = memory[program_counter + 1];
                        if (operand < MEMORY_SIZE && operand >= 0) {
                            int m_op = memory[operand];
                            if (m_op < MEMORY_SIZE && m_op >= 0) {
                                switch (inst) {
                                    case BRZERO_DIRECT:
                                        program_counter = memory[operand];
                                    break;
                                    case BRZERO_INDIRECT:
                                        program_counter = memory[m_op];
                                    break;
                                    case BRZERO_IMMEDIATE:
                                        program_counter = operand;
                                    break;
                                }
                            } else {
                                snprintf(buffer,
                                sizeof(buffer),
                                "Invalid memory address %d",
                                m_op);
                                append_text_to_text_view(textview, buffer);
                            }
                        } else {
                            snprintf(buffer,
                            sizeof(buffer),
                            "Invalid memory address %d",
                            operand);
                            append_text_to_text_view(textview, buffer);
                        }
                    } else {
                        snprintf(buffer,
                        sizeof(buffer),
                        "No operand found for instruction BRZERO at : memory[%d]\n",
                        program_counter + 1);
                           append_text_to_text_view(textview, buffer);
                    }
                } else {
                    program_counter += 2;
                }
            break;

            // BRNEG
            case BRNEG_DIRECT:
            case BRNEG_INDIRECT:
            case BRNEG_IMMEDIATE:
                if (accumulator < 0) {
                    if (program_counter < MEMORY_SIZE - 1) {
                        int operand = memory[program_counter + 1];
                        if (operand < MEMORY_SIZE && operand >= 0) {
                            int m_op = memory[operand];
                            if (m_op < MEMORY_SIZE && m_op >= 0) {
                                switch (inst) {
                                    case BRNEG_DIRECT:
                                        program_counter = memory[operand];
                                    break;
                                    case BRNEG_INDIRECT:
                                        program_counter = memory[m_op];
                                    break;
                                    case BRNEG_IMMEDIATE:
                                        program_counter = operand;
                                    break;
                                }
                            } else {
                                snprintf(buffer,
                                sizeof(buffer),
                                "Invalid memory address %d",
                                m_op);
                                append_text_to_text_view(textview, buffer);
                            }
                        } else {
                            snprintf(buffer,
                            sizeof(buffer),
                            "Invalid memory address %d",
                            operand);
                            append_text_to_text_view(textview, buffer);
                        }
                    } else {
                        snprintf(buffer,
                        sizeof(buffer),
                        "No operand found for instruction BRNEG at : memory[%d]\n",
                        program_counter + 1);
                        append_text_to_text_view(textview, buffer);
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
            case MULT_INDIRECT:
            case MULT_IMMEDIATE:
                if (program_counter < MEMORY_SIZE - 1) {
                    int operand = memory[program_counter + 1];
                    if (operand < MEMORY_SIZE && operand >= 0) {
                        int m_op = memory[operand];
                        if (m_op < MEMORY_SIZE && m_op >= 0) {
                            switch (inst) {
                                case MULT_DIRECT:
                                    accumulator *= memory[operand];
                                break;
                                case MULT_INDIRECT:
                                    accumulator *= memory[m_op];
                                break;
                                case MULT_IMMEDIATE:
                                    accumulator *= operand;
                                break;
                            }
                            program_counter += 2;
                        } else {
                            snprintf(buffer,
                            sizeof(buffer),
                            "Invalid memory address %d",
                            m_op);
                            append_text_to_text_view(textview, buffer);
                        }
                    } else {
                        snprintf(buffer,
                        sizeof(buffer),
                        "Invalid memory address %d",
                        operand);
                        append_text_to_text_view(textview, buffer);
                    }
                } else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "No operand found for instruction MULT at : memory[%d]\n",
                    program_counter + 1);
                    append_text_to_text_view(textview, buffer);
                }
            break;
            // DIV
            case DIV_DIRECT:
            case DIV_INDIRECT:
            case DIV_IMMEDIATE:
                if (program_counter < MEMORY_SIZE - 1) {
                    int operand = memory[program_counter + 1];
                    if (operand < MEMORY_SIZE && operand >= 0) {
                        int m_op = memory[operand];
                        if (m_op < MEMORY_SIZE && m_op >= 0) {
                            switch (inst) {
                                case DIV_DIRECT:
                                    accumulator /= memory[operand];
                                break;
                                case DIV_INDIRECT:
                                    accumulator /= memory[m_op];
                                break;
                                case DIV_IMMEDIATE:
                                    accumulator /= operand;
                                break;
                            }
                            program_counter += 2;
                        } else {
                            snprintf(buffer,
                            sizeof(buffer),
                            "Invalid memory address %d",
                            m_op);
                            append_text_to_text_view(textview, buffer);
                        }
                    } else {
                        snprintf(buffer,
                        sizeof(buffer),
                        "Invalid memory address %d",
                        operand);
                        append_text_to_text_view(textview, buffer);
                    }
                } else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "No operand found for instruction DIV at : memory[%d]\n",
                    program_counter + 1);
                    append_text_to_text_view(textview, buffer);
                }
            break;

            // BR
            case BR_DIRECT:
            case BR_INDIRECT:
            case BR_IMMEDIATE:
                if (program_counter < MEMORY_SIZE - 1) {
                    int operand = memory[program_counter + 1];
                    if (operand < MEMORY_SIZE && operand >= 0) {
                        int m_op = memory[operand];
                        if (m_op < MEMORY_SIZE && m_op >= 0) {
                            switch (inst) {
                                case BR_DIRECT:
                                    program_counter = memory[operand];
                                break;
                                case BR_INDIRECT:
                                    program_counter = memory[m_op];
                                break;
                                case BR_IMMEDIATE:
                                    program_counter = operand;
                                break;
                            }
                        } else {
                            snprintf(buffer,
                            sizeof(buffer),
                            "Invalid memory address %d",
                            m_op);
                            append_text_to_text_view(textview, buffer);
                        }
                    } else {
                        snprintf(buffer,
                        sizeof(buffer),
                        "Invalid memory address %d",
                        operand);
                        append_text_to_text_view(textview, buffer);
                    }
                } else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "No operand found for instruction BR at : memory[%d]\n",
                    program_counter + 1);
                    append_text_to_text_view(textview, buffer);
                }
            break;

            // PUT
            case PUT_DIRECT:
            case PUT_INDIRECT:
            case PUT_IMMEDIATE:
                if ( program_counter < MEMORY_SIZE - 1 ) {
                    int operand = memory[program_counter + 1];
                    if ( operand < MEMORY_SIZE && operand >= 0) {
                        int m_op = memory[ operand ];
                        if ( m_op < MEMORY_SIZE && m_op >= 0 ) {
                            char number[256];
                            switch(inst){
                                case PUT_DIRECT:
                                    snprintf(number, 256, "%d", memory[ operand ]);
                                break;
                                case PUT_INDIRECT:
                                    snprintf(number, 256, "%d", memory[ m_op ]);
                                break;
                                case PUT_IMMEDIATE:
                                    snprintf(number, 256, "%d", operand );
                                break;
                            }
                            append_text_to_text_view(textview, number); 
                            program_counter+=2;
                        }else {
                            snprintf(buffer,
                            sizeof(buffer),
                            "Invalid memory address %d",
                            m_op);
                            append_text_to_text_view(textview, buffer);
                        }
                    }else {
                        snprintf(buffer,
                        sizeof(buffer),
                        "Invalid memory address %d",
                        operand);
                        append_text_to_text_view(textview, buffer);
                    }
                }else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "Invalid memory address %d",
                    program_counter + 1);
                    append_text_to_text_view(textview, buffer);
                }
            break;

            // CALL
            case CALL_DIRECT:
            case CALL_INDIRECT:
            case CALL_IMMEDIATE:
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
                        int m_op = memory[operand];
                        if (m_op < MEMORY_SIZE && m_op >= 0) {
                            switch (inst) {
                                case CALL_DIRECT:
                                    program_counter = memory[ operand ]; 
                                break;
                                case CALL_INDIRECT:
                                    program_counter = memory[ m_op ]; 
                                break;
                                case CALL_IMMEDIATE:
                                    program_counter = operand; 
                                break;
                            }
                        }else {
                            snprintf(buffer,
                            sizeof(buffer),
                            "Invalid memory address %d",
                            m_op);
                            append_text_to_text_view(textview, buffer);
                        }
                    }
                    snprintf(buffer,
                    sizeof(buffer),
                    "Invalid memory address %d",
                    operand);
                    append_text_to_text_view(textview, buffer);
                }else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "Invalid memory address %d",
                    program_counter + 1);
                    append_text_to_text_view(textview, buffer);
                }
            break;

            // COPY
            case COPY_DI:
            case COPY_DIm:
            case COPY_DD:
            case COPY_ID:
            case COPY_II:
            case COPY_IIm:
                if ( program_counter < MEMORY_SIZE - 2 ) {

                    int op1 = memory[program_counter + 1];
                    int op2 = memory[program_counter + 2];
                    int m_op1 = memory [ op1 ];
                    int m_op2 = memory [ op2 ];

                    if ( (op1 < MEMORY_SIZE && op1 >= 0)  
                          && (op2 < MEMORY_SIZE && op2 >= 0)
                          && ( m_op1 < MEMORY_SIZE && m_op1 >= 0 )
                          && ( m_op2 < MEMORY_SIZE && m_op2 >= 0 ) ) {

                        switch (inst) {
                            case COPY_DI:
                                memory[op1] = memory[ memory[op2] ];
                            break;
                            case COPY_DD:
                                memory[op1] = memory[op2];
                            break;
                            case COPY_DIm:
                                memory[op1] = op2;
                            break;
                            case COPY_II: 
                                memory[ memory[op1] ] = memory[ memory[op2] ];
                            break;
                            case COPY_ID: 
                                memory[ memory[op1] ] = memory[op2];
                            break;
                            case COPY_IIm: 
                                memory[ memory[op1] ] = op2;
                            break;
                        }

                        program_counter += 3;

                    } else {
                        snprintf( buffer, sizeof(buffer),
                        "\nInvalid memory address for at least one of the operands "
                        "of instruction COPY at: %d or %d\n", op1, op2);
                        append_text_to_text_view( textview, buffer );
                    }
                } else {
                    snprintf( buffer, sizeof(buffer),
                    "\nNo operand found for instruction copy at"
                    ": memory[ %d ] or memory[ %d ]\n",
                    program_counter + 1, program_counter + 2 );
                    append_text_to_text_view( textview, buffer );
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
            case SUB_INDIRECT:
            case SUB_IMMEDIATE:
                if (program_counter < MEMORY_SIZE - 1) {
                    int operand = memory[program_counter + 1];
                    if (operand < MEMORY_SIZE && operand >= 0) {
                        int m_op = memory[operand];
                        if (m_op < MEMORY_SIZE && m_op >= 0) {
                            switch (inst) {
                                case SUB_DIRECT:
                                    accumulator -= memory[operand];
                                break;
                                case SUB_INDIRECT:
                                    accumulator -= memory[m_op];
                                break;
                                case SUB_IMMEDIATE:
                                    accumulator -= operand;
                                break;
                            }
                            program_counter += 2;
                        } else {
                            snprintf(buffer,
                            sizeof(buffer),
                            "Invalid memory address %d",
                            m_op);
                            append_text_to_text_view(textview, buffer);
                        }
                    } else {
                        snprintf(buffer,
                        sizeof(buffer),
                        "Invalid memory address %d",
                        operand);
                        append_text_to_text_view(textview, buffer);
                    }
                } else {
                    snprintf(buffer,
                    sizeof(buffer),
                    "No operand found for instruction SUB at : memory[%d]\n",
                    program_counter + 1);
                    append_text_to_text_view(textview, buffer);
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

