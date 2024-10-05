
#include "global.h"

char *current_program    = "../program.asm";
char *current_binary     = "../program.elf98";
char *current_macro_out  = "../macrosout.asm";
char *current_linker_out = "../linked_program.elf98";

char *current_parser_error;
GtkBuilder *p_builder = NULL;
GtkTextBuffer *cpe;

