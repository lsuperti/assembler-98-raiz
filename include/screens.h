#ifndef SCREENS_H
#define SCREENS_H

#include <gtk/gtk.h>
#include "architecture.h"
#include "montador.h"
#include "global.h"
#include "program.h"
#include "macro_processor.h"
#include "helper.h"
#include "types.h"
#include "maquina.h"

void change_to_console(GtkWidget *widget, GtkStack *stack);
void change_to_gui(GtkWidget *widget, GtkStack *stack);
void change_to_main(GtkWidget *widget, GtkStack *stack);
void assemble_and_update_file_gui(GtkWidget *widget, gpointer data);
void change_to_file_gui(GtkWidget *widget, GtkStack *stack);

#endif // SCREENS_H
