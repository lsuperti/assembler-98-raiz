#ifndef MAQUINA_H
#define MAQUINA_H

#include <stdint.h>
#include <gtk/gtk.h>
#include "types.h"
#include "stack.h"

enum instructions_t {
    LOAD = 3,
    STORE = 7,
    READ = 12,
    STOP = 11,
    ADD = 2,
    BRZERO = 4,
    BRPOS = 1,
    BRNEG = 5,
    WRITE = 8,
    MULT = 14,
    DIV = 10,
    BR = 00,
    CALL = 15,
    PUT = 20,
    RET = 16
};

void update_inst_pc( GtkBuilder *builder, int inst);
void reset(GtkWidget *widget, gpointer builder);
void execute_current_instruction(void* data);
void update_memory_tree(void* data);
void step(GtkWidget *widget, gpointer data);

#endif // MAQUINA_H

