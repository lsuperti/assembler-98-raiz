#ifndef LIGADOR_H
#define LIGADOR_H

#include "data_structures.h"
#include "montador_types.h"
#include "program.h"
#include "helper.h"

#define OUTPUT_ID 100

typedef struct global
{
    char*  name;
    word_t value;
    bool   data_l;
    UT_hash_handle hh;
} global;

typedef struct tlb_g {
    global *gls;
} tlb_g;

typedef struct extern_t
{
    char* name;
    Vector ps;
    UT_hash_handle hh;
} extern_t;

typedef struct local_t
{
    char* name;
    Vector ps;
    bool   data_l;
    UT_hash_handle hh;
} local_t;

typedef struct modulo
{
    int id;
    Vector    dot_text;
    Vector    dot_data;
    Vector    dot_rodata;
    global    *gls;
    int       num_gls;
    extern_t  *exts;
    int       num_exts;
    local_t   *locals;
    int       num_locals;
    UT_hash_handle hh;
} modulo;

typedef struct paths
{
    GList           *file_paths;
    GtkComboBoxText *combo_box;
    GtkBuilder      *builder;
} paths;

Vector find_all_identifier_pos( program_t *p, token_t *tok );
modulo *read_modulo( char *src );
void print_modulo( modulo *mod );
void on_addmod_activate( GtkMenuItem *m, gpointer data );
void on_removemod_activate( GtkMenuItem *m, gpointer data );
void on_link_activate( GtkMenuItem *m, gpointer data ); 

#endif // LIGADOR_H
