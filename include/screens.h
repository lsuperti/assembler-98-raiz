#ifndef SCREENS_H
#define SCREENS_H

#include <gtk/gtk.h>
#include "architecture.h"

extern void change_to_console(GtkWidget *widget, GtkStack *stack); 
extern void change_to_gui(GtkWidget *widget, GtkStack *stack);
extern void change_to_debugger(GtkWidget *widget, GtkStack *stack);

#endif // SCREENS_H
