#ifndef MAQUINA_H
#define MAQUINA_H

#include <stdint.h>
#include <gtk/gtk.h>
#include "architecture.h"
#include "types.h"
#include "stack.h"

extern bool running;
extern const char * const enum_str[]; 

void update_inst_pc( GtkBuilder *builder, int inst);
void reset(GtkWidget *widget, gpointer builder);
void execute_current_instruction(void* data);
void update_memory_tree(void* data);
void step(GtkWidget *widget, gpointer data);
void run(GtkWidget *widget, gpointer data);
void open_file(GtkButton *button, gpointer user_data);

#endif // MAQUINA_H

