#ifndef MAQUINA_H
#define MAQUINA_H

#include <stdint.h>
#include <gtk/gtk.h>
#include "types.h"
#include "stack.h"

enum instructions_t {
    LOAD_DIRECT = 3,
    LOAD_INDIRECT = 50,
    LOAD_IMMEDIATE = 51,
    ADD_DIRECT = 2,
    ADD_INDIRECT = 52,
    ADD_IMMEDIATE = 53,
    STORE_DIRECT = 7,
    STORE_INDIRECT = 54,
    READ_DIRECT = 12,
    READ_INDIRECT = 55,
    STOP = 11,
    BRZERO_DIRECT = 4,
    BRZERO_INDIRECT = 56,
    BRPOS_DIRECT = 1,
    BRPOS_INDIRECT = 57,
    BRNEG_DIRECT = 5,
    BRNEG_INDIRECT = 58,
    WRITE_DIRECT = 8,
    WRITE_INDIRECT = 59,
    MULT_DIRECT = 14,
    MULT_INDIRECT = 60,
    MULT_IMMEDIATE = 61,
    SUB_DIRECT = 6,
    SUB_INDIRECT = 62,
    SUB_IMMEDIATE = 63,
    DIV_DIRECT = 10,
    DIV_INDIRECT = 64,
    DIV_IMMEDIATE = 65,
    BR_DIRECT = 00,
    BR_INDIRECT = 66,
    BR_IMMEDIATE = 67,
    CALL_DIRECT = 15,
    CALL_INDIRECT = 68,
    PUT_DIRECT = 20,
    PUT_INDIRECT = 69,
    PUT_IMMEDIATE = 70,
    RET = 16,
    PC  = 100,
    RI  = 101,
    SP  = 102,
    UNKNOWN = 103
};

extern const char * const enum_str[]; 

void update_inst_pc( GtkBuilder *builder, int inst);
void reset(GtkWidget *widget, gpointer builder);
void execute_current_instruction(void* data);
void update_memory_tree(void* data);
void step(GtkWidget *widget, gpointer data);
void run(GtkWidget *widget, gpointer data);

#endif // MAQUINA_H

