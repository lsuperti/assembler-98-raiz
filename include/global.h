#ifndef GLOBAL_H
#define GLOBAL_H

#include <string.h>
#include <stdint.h>
#include <gtk/gtk.h>

#define TEXT_SEGMENT_START 256
#define DATA_SEGMENT_START 767
#define MAX_LINE_LENGTH    512
#define magic ("7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00\n")

extern char *current_program; 
extern char *current_binary; 
extern char *current_parser_error;
extern GtkBuilder *p_builder;
extern GtkTextBuffer *cpe;

#endif // GLOBAL_H
