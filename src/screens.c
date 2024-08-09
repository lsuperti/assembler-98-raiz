
#include "screens.h"

void change_to_console(GtkWidget *widget, GtkStack *stack) {
    gtk_stack_set_visible_child_name(stack, "WINDOW_FIXED_MOP0" );
    mop = MOP0;
}

void change_to_gui(GtkWidget *widget, GtkStack *stack){
    gtk_stack_set_visible_child_name(stack, "WINDOW_FIXED_MOP2" );
    mop = MOP2;
}

void change_to_main(GtkWidget *widget, GtkStack *stack){
    gtk_stack_set_visible_child_name(stack, "WINDOW_FIXED_START" );
    mop = MOP_NOT_DEFINED;
}
