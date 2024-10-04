#ifndef LIGADOR_H
#define LIGADOR_H

#include "data_structures.h"
#include "montador_types.h"
#include "program.h"

typedef struct global
{
    char* name;
    Vector ps;
}global;

typedef struct extern_t
{
    char* name;
    Vector ps;
}extern_t;

typedef struct modulo
{
    Vector    dot_text;
    Vector    dot_data;
    Vector    dot_rodata;
    global    *gls;
    extern_t  *exts;
} modulo;

typedef struct paths
{
    GList           *file_paths;
    GtkComboBoxText *combo_box;
} paths;

Vector find_all_identifier_pos( program_t *p, token_t *tok );
modulo *read_modulo( char *src );
void on_addmod_activate( GtkMenuItem *m, gpointer data );
void on_removemod_activate( GtkMenuItem *m, gpointer data );
void on_link_activate( GtkMenuItem *m, gpointer data ); 

#endif // LIGADOR_H
