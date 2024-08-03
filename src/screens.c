
#include "screens.h"

void change_to_console(GtkWidget *widget, GtkStack *stack) {
    gtk_stack_set_visible_child_name(stack, "WINDOW_FIXED_MOP0" );
    mop = 0;
}

void change_to_gui(GtkWidget *widget, GtkStack *stack){
    gtk_stack_set_visible_child_name(stack, "WINDOW_FIXED_MOP1" );
    mop = 1;
}

void change_to_debugger(GtkWidget *widget, GtkStack *stack){
    gtk_stack_set_visible_child_name(stack, "WINDOW_FIXED_MOP2" );
    mop = 2;
}

