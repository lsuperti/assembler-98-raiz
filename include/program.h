#ifndef PROGRAM_H
#define PROGRAM_H

#include "architecture.h"
#include "global.h"
#include <string.h>
#include <stdbool.h>
#include "helper.h"
#include <stdio.h>
#include <inttypes.h>
#include <assert.h>
#include <gtk/gtk.h>
#include "maquina.h"

void load_program(void); 
void on_load_activate ( GtkMenuItem *m );

#endif // PROGRAM_H
       
